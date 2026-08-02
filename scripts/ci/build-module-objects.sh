#!/usr/bin/env bash
# L0 build in the current directory; write 88x2bu link objects (one path per line).
# Usage: cd /path/to/driver && build-module-objects.sh OUTPUT_FILE
# Requires KDIR (default /opt/linux) — same as the L0 gate.
set -euo pipefail

OUT="${1:?usage: build-module-objects.sh OUTPUT_FILE}"
KDIR="${KDIR:-/opt/linux}"
LLVM="${LLVM:-1}"
BUILD_DIR="$(pwd)"
SCRIPT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"

if [ ! -f "$BUILD_DIR/Makefile" ]; then
	echo "build-module-objects: no Makefile in $BUILD_DIR" >&2
	exit 1
fi

if [ ! -d "$KDIR" ]; then
	echo "build-module-objects: KDIR=$KDIR not found" >&2
	echo "Set KDIR to a Rust-enabled kernel build tree (see docs/rust-migration/dev-environment.md)." >&2
	exit 1
fi

export LIBCLANG_PATH="${LIBCLANG_PATH:-/usr/lib/llvm-18/lib}"

echo "build-module-objects: building 88x2bu.mod (cwd=$BUILD_DIR KDIR=$KDIR)..." >&2
make clean
make KDIR="$KDIR" LLVM="$LLVM" -j"$(nproc)"

MOD="$BUILD_DIR/88x2bu.mod"
if [ ! -f "$MOD" ]; then
	echo "build-module-objects: $MOD not found after build" >&2
	exit 1
fi

"$SCRIPT_ROOT/scripts/ci/extract-module-objects.sh" "$MOD" "$OUT"
echo "build-module-objects: wrote $(wc -l < "$OUT") objects" >&2
