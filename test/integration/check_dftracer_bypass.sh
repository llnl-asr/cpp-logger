#!/usr/bin/env bash
# Usage: check_dftracer_bypass.sh <deps-prefix> <dftracer-lib-dir>
# Logs 20 lines through cpp-logger under the DFTracer preload and fails if the
# trace holds any fputs or fflush event, which means GOTCHA saw the logger.
set -euo pipefail

deps="$1"
libdir="$2"
here="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
work="$(mktemp -d)"

g++ -std=c++11 -I"$deps/include" "$here/dftracer_bypass_probe.cpp" \
  -L"$deps/lib64" -lcpp-logger -ldl -o "$work/probe"

export LD_LIBRARY_PATH="$deps/lib64:$libdir"
LD_PRELOAD="$libdir/libdftracer_preload_dbg.so" \
  DFTRACER_LOG_LEVEL=DEBUG DFTRACER_INIT=PRELOAD DFTRACER_ENABLE=1 \
  DFTRACER_DATA_DIR="$work" DFTRACER_LOG_FILE="$work/trace" \
  DFTRACER_ENABLE_AGGREGATION=0 "$work/probe" "$work/log.txt" >"$work/out.txt" 2>&1

count_events() {
  { grep -oE "\"name\":\"$1\",\"cat\":\"STDIO\"" "$work/out.txt" || true; } | wc -l | tr -d ' '
}

lines="$(wc -l <"$work/log.txt" | tr -d ' ')"
fopen="$(count_events fopen)"
fputs="$(count_events fputs)"
fflush="$(count_events fflush)"
echo "log lines=$lines traced fopen=$fopen fputs=$fputs fflush=$fflush"

[ "$lines" -eq 20 ] || { echo "FAIL: logger wrote $lines lines, expected 20" >&2; exit 1; }
[ "$fopen" -eq 1 ] || { echo "FAIL: DFTracer did not trace the probe" >&2; exit 1; }
if [ "$fputs" -ne 0 ] || [ "$fflush" -ne 0 ]; then
  echo "FAIL: GOTCHA traced the logger output" >&2
  exit 1
fi
echo "OK: logger output bypasses GOTCHA"
