#!/usr/bin/env bash
# Usage: run_dftracer_integration.sh <cpp-logger-dir> <dftracer-dir>
# Builds DFTracer against the checked-out cpp-logger commit, runs the DFTracer
# tests, then runs the bypass probe. Meant for the manylinux_2_28 image, as root.
set -euo pipefail

cpp_logger_dir="$(cd -- "$1" && pwd)"
dftracer_dir="$(cd -- "$2" && pwd)"
deps=/opt/dftracer-deps
ref=ci-under-test

dnf install -y -q git zlib-devel jq >/dev/null
# shellcheck disable=SC1091
source /opt/rh/gcc-toolset-14/enable
export PATH=/opt/python/cp311-cp311/bin:$PATH
pip install -q "cmake>=3.24" ninja
git config --global --add safe.directory '*'

git -C "$cpp_logger_dir" branch -f "$ref" HEAD
url="file://$cpp_logger_dir"

python3 - "$dftracer_dir" "$url" "$ref" <<'PY'
import re
import sys

root, url, ref = sys.argv[1:4]

def patch(path, pattern, replacement):
    text = open(path).read()
    text, count = re.subn(pattern, replacement, text, flags=re.M)
    if count != 1:
        sys.exit(f"patch failed for {path}: {count} matches")
    open(path, "w").write(text)

patch(f"{root}/dependency/manifest.txt", r"^cpp-logger .*$",
      f"cpp-logger   {ref}   {url}")
patch(f"{root}/dependency/CMakeLists.txt",
      r"(dftracer_install_external_project\(cpp-logger \S+ \"CPP_LOGGER\") \S+ \S+",
      rf"\1 {url} {ref}")
PY

export DFTRACER_PROJECT_DIR="$dftracer_dir" DFTRACER_DEPS_PREFIX="$deps" \
  DFTRACER_DEPS_WORKDIR=/tmp/dftracer-deps
bash "$dftracer_dir/scripts/wheel/build_deps.sh"

cmake -S "$dftracer_dir" -B "$dftracer_dir/build" \
  $(cat "$deps/cmake-args.txt") \
  -DDFTRACER_ENABLE_TESTS=ON -DDFTRACER_BUILD_PYTHON_BINDINGS=OFF \
  -DCMAKE_BUILD_TYPE=Release
cmake --build "$dftracer_dir/build" -j "$(nproc)"

libdir="$(dirname "$(find "$dftracer_dir/build" -name libdftracer_preload_dbg.so | head -1)")"
export LD_LIBRARY_PATH="$deps/lib64:$libdir"
(cd "$dftracer_dir/build" && DFTRACER_BIND_SIGNALS=1 ctest -E test_py --output-on-failure)

bash "$cpp_logger_dir/test/integration/check_dftracer_bypass.sh" "$deps" "$libdir"
