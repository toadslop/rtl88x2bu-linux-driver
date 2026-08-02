#!/usr/bin/env bash
# Deprecated wrapper — use build-module-objects.sh instead.
# Writes the link object list to OUTPUT (default: stdout).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
OUT="${1:-}"

echo "update-migration-baseline.sh: use scripts/ci/build-module-objects.sh (see --help in migration-progress workflow)" >&2

if [ -n "$OUT" ]; then
	( cd "$ROOT" && "$ROOT/scripts/ci/build-module-objects.sh" "$OUT" )
else
	TMP="$(mktemp)"
	trap 'rm -f "$TMP"' EXIT
	( cd "$ROOT" && "$ROOT/scripts/ci/build-module-objects.sh" "$TMP" )
	cat "$TMP"
fi
