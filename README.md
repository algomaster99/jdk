# AIJitAgent

A proof of concept in which an LLM agent steers C2 at runtime. With
`-XX:+AIJitAgent`, a hidden JVM thread samples the hottest C2-compiled methods,
sends a JSON profile to an agent command, validates the hints it returns
(`inline <method> <callee>`, `unroll <method> <limit>`), installs them as
compiler directives and recompiles the methods. C2 still generates all the code.

## Build

A release build, configured with its own configuration name; `configure` needs
a boot JDK (we used JDK 27):

```
bash configure --with-conf-name=linux-x86_64-ai-jit
make images CONF=linux-x86_64-ai-jit
```

## Run

```
src/utils/aijit/run.sh [seconds]        # default 60
```

`run.sh` compiles `Simulation.java` and runs it twice with the JDK built above:
once as a baseline and once with `-XX:+AIJitAgent -Xlog:aijit`. Each run prints
the throughput (ops/s) of both benchmark methods every second, the steady-state
mean of the last half, and the number of wrong results.

The agent calls Claude through the `claude` CLI, so `claude` must be installed
and logged in (or `CLAUDE_CODE_OAUTH_TOKEN` set). Other settings:

| Variable            | Effect                                                          |
|---------------------|-----------------------------------------------------------------|
| `ANTHROPIC_API_KEY` | call the Messages API directly instead of the `claude` CLI      |
| `AIJIT_MODEL`       | Claude model id (default `claude-opus-5-5`)                     |
| `AIJIT_MOCK=1`      | skip the LLM and use a fixed heuristic, to test the plumbing    |
| `JAVA`              | `java` to use (default: `build/linux-x86_64-ai-jit/images/jdk`) |

Each agent round leaves its input and output in
`/tmp/aijit-<pid>-<round>-profile.json` and `/tmp/aijit-<pid>-<round>-hints.txt`.

## Files

- `src/utils/aijit/Simulation.java`: the benchmark. `inline` runs a hot callee that C2 does not
  inline because of its size; `control` runs a loop the agent should leave alone.
- `src/utils/aijit/aijit_agent.py`: the agent command; holds the system prompt.
- `src/utils/aijit/run.sh`: builds and runs the benchmark, without and with the agent.
- `src/hotspot/share/runtime/aiJitAgent.cpp`: the HotSpot side (agent thread,
  profile, hint validation, directives and recompilation).

For building the JDK in general, see [doc/building.md](doc/building.md) and
<https://openjdk.org/>.
