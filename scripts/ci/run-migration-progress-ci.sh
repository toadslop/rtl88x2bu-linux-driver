#!/usr/bin/env bash
# CI entry: L0-build link set at HEAD, then print progress report.
# Usage: run-migration-progress-ci.sh [COMPARE_REF]
# Expects KDIR inside the L0 container. Run from the repository root.
#
# Base-ref metrics use git Makefile inference (no second full L0 build).
# Set MIGRATION_PROGRESS_FULL_BASE_BUILD=1 to force a worktree L0 build at COMPARE_REF.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

# Git 2.35+ rejects repos owned by another UID (typical for docker -v "$PWD:/driver").
git config --global --add safe.directory "$ROOT"

COMPARE_REF="${1:-}"
HEAD_OBJECTS="$(mktemp)"
BASE_OBJECTS=""
WORKTREE=""

cleanup() {
	rm -f "$HEAD_OBJECTS"
	[ -n "$BASE_OBJECTS" ] && rm -f "$BASE_OBJECTS"
	if [ -n "$WORKTREE" ] && [ -d "$WORKTREE" ]; then
		git worktree remove -f "$WORKTREE" 2>/dev/null || rm -rf "$WORKTREE"
	fi
}
trap cleanup EXIT

"$ROOT/scripts/ci/build-module-objects.sh" "$HEAD_OBJECTS"

ARGS=(--module-objects "$HEAD_OBJECTS")

if [ -n "$COMPARE_REF" ]; then
	ARGS+=(--compare-ref "$COMPARE_REF")
	if [ "${MIGRATION_PROGRESS_FULL_BASE_BUILD:-}" = "1" ]; then
		BASE_OBJECTS="$(mktemp)"
		WORKTREE="$(mktemp -d)"
		git worktree add --detach "$WORKTREE" "$COMPARE_REF"
		(
			cd "$WORKTREE"
			"$ROOT/scripts/ci/build-module-objects.sh" "$BASE_OBJECTS"
		)
		ARGS+=(--compare-module-objects "$BASE_OBJECTS")
	fi
fi

exec python3 "$ROOT/scripts/ci/compute-migration-progress.py" "${ARGS[@]}"
