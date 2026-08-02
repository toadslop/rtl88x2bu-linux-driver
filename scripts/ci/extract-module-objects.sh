#!/usr/bin/env bash
# Write repo-relative object paths from an existing 88x2bu.mod (one per line).
# Usage: extract-module-objects.sh 88x2bu.mod [OUTPUT_FILE]
set -euo pipefail

MOD="${1:?usage: extract-module-objects.sh 88x2bu.mod [OUTPUT_FILE]}"
OUT="${2:-}"

if [ ! -f "$MOD" ]; then
	echo "extract-module-objects: $MOD not found" >&2
	exit 1
fi

BUILD_DIR="$(cd "$(dirname "$MOD")" && pwd)"
if [ -n "$OUT" ]; then
	sed "s|^${BUILD_DIR}/||" "$MOD" > "$OUT"
else
	sed "s|^${BUILD_DIR}/||" "$MOD"
fi
