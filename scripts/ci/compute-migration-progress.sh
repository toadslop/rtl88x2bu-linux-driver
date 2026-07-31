#!/usr/bin/env bash
# Wrapper for compute-migration-progress.py (PR comment / local report).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec python3 "$ROOT/scripts/ci/compute-migration-progress.py" "$@"
