#!/usr/bin/env bash
# Usage: script/formatting/check-formatting.sh [clang-format-executable]
set -uo pipefail

source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

clang_format="${1:-clang-format}"
require_clang_format "$clang_format"

collect_sources
if [ "${#SOURCES[@]}" -eq 0 ]; then
  echo "No C/C++ files to check."
  exit 0
fi

echo "Checking formatting of ${#SOURCES[@]} C/C++ files"
if "$clang_format" --dry-run -Werror "${SOURCES[@]}"; then
  echo "All checks passed successfully."
else
  echo "Some formatting checks failed. Run script/formatting/autoformat.sh to fix them." >&2
  exit 2
fi
