#!/usr/bin/env bash
# Thin wrapper around find_work.py for agent workflows.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec python3 "$ROOT/scripts/workflow/find_work.py" "$@"
