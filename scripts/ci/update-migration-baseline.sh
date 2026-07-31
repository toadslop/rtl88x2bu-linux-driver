#!/usr/bin/env bash
# Regenerate scripts/ci/migration-module-objects.txt from a local L0 build.
# Requires a Rust-enabled kernel tree (KDIR) — same as L0 gate.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

KDIR="${KDIR:-/opt/linux}"
LLVM="${LLVM:-1}"

if [ ! -d "$KDIR" ]; then
	echo "update-migration-baseline: KDIR=$KDIR not found" >&2
	echo "Set KDIR to a Rust-enabled kernel build tree (see docs/rust-migration/dev-environment.md)." >&2
	exit 1
fi

export LIBCLANG_PATH="${LIBCLANG_PATH:-/usr/lib/llvm-18/lib}"

echo "Building 88x2bu.mod (KDIR=$KDIR LLVM=$LLVM)..."
make clean
make KDIR="$KDIR" LLVM="$LLVM" -j"$(nproc)"

MOD="$ROOT/88x2bu.mod"
if [ ! -f "$MOD" ]; then
	echo "update-migration-baseline: $MOD not found after build" >&2
	exit 1
fi

OUT="$ROOT/scripts/ci/migration-module-objects.txt"
# Normalize to repo-relative paths for CI portability.
sed "s|^$ROOT/||" "$MOD" > "$OUT"
echo "Wrote $(wc -l < "$OUT") objects to ${OUT#"$ROOT"/}"
