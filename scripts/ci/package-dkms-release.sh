#!/usr/bin/env bash
# Build a DKMS source tarball for GitHub Releases (R1).
# Usage: ./scripts/ci/package-dkms-release.sh
# Outputs: dist/rtl88x2bu-<ver>-dkms.tar.gz
# Sets GITHUB_OUTPUT keys version, tarball, tag when run in Actions.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO_ROOT"

GIT_SHA="${GIT_SHA:-$(git rev-parse HEAD)}"
SHORT_SHA="$(git rev-parse --short=7 "$GIT_SHA")"
UTC_DATE="$(date -u +%Y.%m.%d)"
VER="5.13.1.migration.${UTC_DATE}.${SHORT_SHA}"
TAG="v${VER}"

OUT_DIR="${OUT_DIR:-${REPO_ROOT}/dist}"
mkdir -p "$OUT_DIR"

WORK="$(mktemp -d)"
cleanup() { rm -rf "$WORK"; }
trap cleanup EXIT

git archive --format=tar "$GIT_SHA" | tar -x -C "$WORK"
sed -i "s/@PKGVER@/${VER}/" "${WORK}/dkms.conf"

TARBALL_NAME="rtl88x2bu-${VER}-dkms.tar.gz"
TARBALL="${OUT_DIR}/${TARBALL_NAME}"
tar -czf "$TARBALL" -C "$WORK" .

echo "Packaged ${TARBALL} (version ${VER})"

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "version=${VER}"
    echo "tarball=${TARBALL}"
    echo "tag=${TAG}"
  } >>"$GITHUB_OUTPUT"
fi
