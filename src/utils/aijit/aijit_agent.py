#!/usr/bin/env python3
"""Agent command for -XX:+AIJitAgent.

The JVM runs:  aijit_agent.py <profile.json> <hints.txt>

It reads the C2 profile written by AIJitAgentThread, asks Claude which
hints to apply, and writes them one per line:

    # reason
    inline <method> <callee>
    unroll <method> <limit>

The JVM validates every hint again, so a bad answer is rejected or, at
worst, makes code slower; it cannot make it wrong.

Environment:
    ANTHROPIC_API_KEY  call the Messages API directly; without it the
                       'claude' CLI is used, which also accepts a Claude
                       subscription login or CLAUDE_CODE_OAUTH_TOKEN
    AIJIT_BACKEND=opencode
                       use 'opencode run' with the provider login stored in
                       opencode (e.g. DeepSeek), instead of Claude
    AIJIT_MODEL        default claude-opus-5-5, or deepseek/deepseek-v4-pro
                       with the opencode backend (provider/model)
    AIJIT_MOCK=1       skip the LLM and use a fixed heuristic (plumbing test)
"""
import json
import os
import re
import subprocess
import sys
import tempfile
import time
import urllib.request

SYSTEM_PROMPT = """\
You tune the HotSpot C2 JIT compiler for a running Java program.

You get a JSON profile of the hottest C2-compiled methods, sampled while the
program runs. For each method: its share of samples, bytecode size, whether it
has loops, and every call site in the method and in the code C2 already
inlined into it, with profiled call counts, callee bytecode sizes, whether the
callee has loops, and whether C2 inlined the callee. Calls that C2 replaces
with hand-written code (intrinsics) are not listed. C2's default thresholds
are in c2_defaults.

You may emit two kinds of hints, one per line:
  inline <method> <callee>   force C2 to inline <callee> when compiling <method>
  unroll <method> <limit>    LoopUnrollLimit (max loop body size in IR nodes,
                             8..600) when compiling <method>
<method> must be a "name" from "methods" and <callee> a "callee" listed in that
method's "calls", copied exactly.

Do not repeat anything in "applied_hints". Emitting no hints is fine.

Before each hint write one line starting with '#' giving the reason.
Output nothing else: no prose, no markdown."""

HINT_LINE = re.compile(r"^(#.*|(inline|unroll) \S+ \S+)$")


def log(msg):
    print(f"[aijit-agent] {msg}", file=sys.stderr, flush=True)


BACKEND = os.environ.get("AIJIT_BACKEND", "")
MODEL = os.environ.get("AIJIT_MODEL",
                       "deepseek/deepseek-v4-pro" if BACKEND == "opencode" else "claude-opus-5-5")


CLAUDE_TRANSCRIPT_DIR = "aijit_claudecode_transcripts"   # relative to the JVM's working directory


def export_claude_session(session_id, round_no, cwd):
    # claude -p stores the session as JSONL under ~/.claude/projects/<cwd with / -> ->/.
    project = re.sub(r"[^A-Za-z0-9]", "-", os.path.realpath(cwd))
    source = os.path.join(os.path.expanduser("~/.claude/projects"), project, f"{session_id}.jsonl")
    with open(source) as f:
        entries = [json.loads(line) for line in f if line.strip()]
    os.makedirs(CLAUDE_TRANSCRIPT_DIR, exist_ok=True)
    path = os.path.join(CLAUDE_TRANSCRIPT_DIR, f"round-{round_no}-{session_id}.json")
    with open(path, "w") as f:
        json.dump(entries, f, indent=2)
    return path


def ask_claude_cli(profile_text, round_no=0):
    # Plain completion: no tools, settings, hooks or MCP servers, no saved
    # session, and a cwd without a CLAUDE.md or project memory.
    cwd = tempfile.gettempdir()
    result = subprocess.run(
        ["claude", "-p", "--model", MODEL, "--system-prompt", SYSTEM_PROMPT,
         "--tools", "", "--setting-sources", "", "--strict-mcp-config",
         "--output-format", "json"],
        input=profile_text, capture_output=True, text=True, timeout=300, cwd=cwd)
    if result.returncode != 0:
        raise RuntimeError(f"claude -p exited with {result.returncode}: {result.stderr.strip()[:500]}")
    reply = json.loads(result.stdout)
    try:
        log(f"transcript: {export_claude_session(reply['session_id'], round_no, cwd)}")
    except Exception as e:   # the transcript is optional, the hints are not
        log(f"transcript export failed: {e}")
    return reply["result"]


TRANSCRIPT_DIR = "aijit_opencode_transcripts"   # relative to the JVM's working directory


def export_opencode_session(tmp, session_id, round_no):
    exported = subprocess.run(["opencode", "export", session_id, "--pure"],
                              capture_output=True, text=True, timeout=60, cwd=tmp)
    if exported.returncode != 0:
        raise RuntimeError(exported.stderr.strip()[:500])
    os.makedirs(TRANSCRIPT_DIR, exist_ok=True)
    path = os.path.join(TRANSCRIPT_DIR, f"round-{round_no}-{session_id}.json")
    with open(path, "w") as f:
        f.write(exported.stdout)
    return path


def ask_opencode(profile_text, round_no=0):
    # opencode has no system-prompt flag, so the prompt and the profile go in
    # one attached file. A temp cwd keeps project config and AGENTS.md out.
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, "profile.txt")
        with open(path, "w") as f:
            f.write(SYSTEM_PROMPT + "\n\nProfile:\n" + profile_text)
        result = subprocess.run(
            ["opencode", "run", "--pure", "--format", "json", "-m", MODEL,
             "Follow the instructions in the attached file and answer with the hint lines only.",
             "-f", path],
            capture_output=True, text=True, timeout=300, cwd=tmp)
        # The JSON events carry the session id and the answer text. 'session
        # list' is global and not ordered by recency, so it cannot be used.
        events = []
        for line in result.stdout.splitlines():
            try:
                events.append(json.loads(line))
            except ValueError:
                pass
        session_id = next((e["sessionID"] for e in events if "sessionID" in e), None)
        if session_id:
            try:
                log(f"transcript: {export_opencode_session(tmp, session_id, round_no)}")
            except Exception as e:   # the transcript is optional, the hints are not
                log(f"transcript export failed: {e}")
    if result.returncode != 0:
        errors = "; ".join(str(e["error"].get("data", e["error"])) for e in events if e.get("type") == "error")
        raise RuntimeError(f"opencode run exited with {result.returncode}: {errors or result.stderr.strip()[:500]}")
    return "\n".join(e["part"]["text"] for e in events if e.get("type") == "text")


def ask_claude_api(profile_text):
    request = urllib.request.Request(
        "https://api.anthropic.com/v1/messages",
        data=json.dumps({
            "model": MODEL,
            "max_tokens": 2048,
            "system": SYSTEM_PROMPT,
            "messages": [{"role": "user", "content": profile_text}],
        }).encode(),
        headers={
            "x-api-key": os.environ["ANTHROPIC_API_KEY"],
            "anthropic-version": "2023-06-01",
            "content-type": "application/json",
        },
    )
    with urllib.request.urlopen(request, timeout=120) as response:
        body = json.load(response)
    return "".join(block["text"] for block in body["content"] if block["type"] == "text")


def mock_hints(profile):
    """Fixed heuristic: inline hot, too-big callees; unroll large call-free loop code."""
    defaults = profile["c2_defaults"]
    applied = set(profile["applied_hints"])
    lines = []
    for m in profile["methods"]:
        for call in m["calls"]:
            hint = f"inline {m['name']} {call['callee']}"
            if (not call["inlined"] and call["count"] > 10_000 and hint not in applied
                    and defaults["FreqInlineSize"] < call["callee_bytecode_size"] <= 2000):
                lines += [f"# hot call to a {call['callee_bytecode_size']}-byte callee was not inlined", hint]
        loop_sizes = [m["bytecode_size"]] if m["has_loops"] else []
        loop_sizes += [c["callee_bytecode_size"] for c in m["calls"] if c["inlined"] and c["callee_has_loops"]]
        has_calls = any(not c["inlined"] and c["count"] > 0 for c in m["calls"])
        hint = f"unroll {m['name']} {2 * defaults['LoopUnrollLimit']}"
        if loop_sizes and max(loop_sizes) >= 64 and not has_calls and hint not in applied:
            lines += [f"# {max(loop_sizes)}-bytecode loop method with no calls, body likely above LoopUnrollLimit", hint]
    return "\n".join(dict.fromkeys(lines))   # drop duplicate lines, keep order


def main():
    profile_path, hints_path = sys.argv[1], sys.argv[2]
    with open(profile_path) as f:
        profile_text = f.read()
    profile = json.loads(profile_text)

    start = time.time()
    if os.environ.get("AIJIT_MOCK") == "1":
        source, answer = "mock", mock_hints(profile)
    elif BACKEND == "opencode":
        source, answer = f"{MODEL} (opencode)", ask_opencode(profile_text, profile['round'])
    elif os.environ.get("ANTHROPIC_API_KEY"):
        source, answer = f"{MODEL} (API)", ask_claude_api(profile_text)
    else:
        source, answer = f"{MODEL} (claude CLI)", ask_claude_cli(profile_text, profile['round'])
    hints = [line.strip() for line in answer.splitlines() if HINT_LINE.match(line.strip())]

    with open(hints_path, "w") as f:
        f.write("\n".join(hints) + "\n")
    count = sum(1 for h in hints if not h.startswith("#"))
    log(f"round {profile['round']}: {source} returned {count} hint(s) in {time.time() - start:.1f}s -> {hints_path}")


if __name__ == "__main__":
    main()
