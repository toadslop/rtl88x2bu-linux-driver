#!/usr/bin/env bash
# Wrapper for compute-migration-progress.py (PR comment / local report).
# When --module-objects is omitted, uses 88x2bu.mod from a recent build or runs L0.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

has_module_objects=false
for arg in "$@"; do
	case "$arg" in
	--module-objects) has_module_objects=true ;;
	esac
done

if [ "$has_module_objects" = false ]; then
	OBJECTS="$(mktemp)"
	trap 'rm -f "$OBJECTS"' EXIT
	if [ -f "$ROOT/88x2bu.mod" ]; then
		"$ROOT/scripts/ci/extract-module-objects.sh" "$ROOT/88x2bu.mod" "$OBJECTS"
	elif [ -d "${KDIR:-/opt/linux}" ]; then
		( cd "$ROOT" && "$ROOT/scripts/ci/build-module-objects.sh" "$OBJECTS" )
	else
		echo "compute-migration-progress: pass --module-objects, or build 88x2bu.ko first (KDIR=/opt/linux)." >&2
		exit 1
	fi
	set -- --module-objects "$OBJECTS" "$@"
fi

exec python3 "$ROOT/scripts/ci/compute-migration-progress.py" "$@"
