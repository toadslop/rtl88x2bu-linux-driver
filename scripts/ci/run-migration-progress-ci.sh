#!/usr/bin/env bash
# CI entry: L0-build link set at HEAD, then print link-mix report.
# Usage: run-migration-progress-ci.sh
# Expects KDIR inside the L0 container. Run from the repository root.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

git config --global --add safe.directory "$ROOT" 2>/dev/null || true

HEAD_OBJECTS="$(mktemp)"
trap 'rm -f "$HEAD_OBJECTS"' EXIT

"$ROOT/scripts/ci/build-module-objects.sh" "$HEAD_OBJECTS"
exec python3 "$ROOT/scripts/ci/compute-migration-progress.py" --module-objects "$HEAD_OBJECTS"