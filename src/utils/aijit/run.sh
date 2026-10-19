#!/bin/sh
#
# Runs Simulation without and with -XX:+AIJitAgent on a JDK built from this branch.
#
#   JAVA               java to use (default: build/linux-x86_64-ai-jit/images/jdk/bin/java)
#   AIJIT_MOCK=1       use the agent's fixed heuristic instead of calling Claude
#   ANTHROPIC_API_KEY  call the Messages API; otherwise the 'claude' CLI is used
#                      (logged-in subscription or CLAUDE_CODE_OAUTH_TOKEN)
#   AIJIT_MODEL        Claude model id (default claude-opus-5-5)
#
# Usage: run.sh [seconds]   (default 60, leaving room for LLM latency)
set -e
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../../.." && pwd)
JAVA=${JAVA:-$root/build/linux-x86_64-ai-jit/images/jdk/bin/java}
duration=${1:-60}
classes=$(mktemp -d)
"$(dirname "$JAVA")/javac" -d "$classes" "$here/Simulation.java"

echo "== baseline"
"$JAVA" -cp "$classes" Simulation "$duration"

echo
echo "== with AIJitAgent"
"$JAVA" -XX:+UnlockExperimentalVMOptions -XX:+AIJitAgent \
        -XX:AIJitAgentCommand="python3 -I $here/aijit_agent.py" \
        -Xlog:aijit -cp "$classes" Simulation "$duration"
