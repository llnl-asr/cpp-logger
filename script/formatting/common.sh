#!/usr/bin/env bash
# Shared by autoformat.sh and check-formatting.sh.

SUPPORTED_CLANG_FORMAT_VERSION="19.1.7"
FORMAT_DIRS=(src include test)

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"

require_clang_format() {
  local exe="$1"
  if ! command -v "$exe" >/dev/null 2>&1; then
    echo "clang-format not found: $exe" >&2
    exit 1
  fi
  local version
  version="$("$exe" --version | sed -n 's/.*version \([0-9][0-9.]*\).*/\1/p')"
  if [ "$version" != "$SUPPORTED_CLANG_FORMAT_VERSION" ]; then
    echo "WARNING: .clang-format is pinned for clang-format $SUPPORTED_CLANG_FORMAT_VERSION." >&2
    echo "         You run $version, so the result may differ." >&2
  fi
}

# Fills the global array SOURCES with every C/C++ file under FORMAT_DIRS.
collect_sources() {
  SOURCES=()
  local dir file
  for dir in "${FORMAT_DIRS[@]}"; do
    [ -d "$ROOT_DIR/$dir" ] || continue
    while IFS= read -r -d '' file; do
      SOURCES+=("$file")
    done < <(find "$ROOT_DIR/$dir" -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) -print0)
  done
}
