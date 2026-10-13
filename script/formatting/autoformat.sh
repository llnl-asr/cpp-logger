#!/usr/bin/env bash
# Usage: script/formatting/autoformat.sh [clang-format-executable]
set -euo pipefail

source "$(dirname -- "${BASH_SOURCE[0]}")/common.sh"

clang_format="${1:-clang-format}"
require_clang_format "$clang_format"

collect_sources
if [ "${#SOURCES[@]}" -eq 0 ]; then
  echo "No C/C++ files to format."
  exit 0
fi

echo "Formatting ${#SOURCES[@]} C/C++ files"
"$clang_format" -i "${SOURCES[@]}"
