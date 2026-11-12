#!/bin/bash
# T23: panic-site inventory for kernel Rust exports (see test-23 spec).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
exec python3 "${ROOT}/scripts/ci/panic_sites.py" "$@"
