#!/bin/bash
# T18: Read-only check that master requires L0/L1/L2 status checks (issue #1795).
set -euo pipefail

REPO="${GITHUB_REPOSITORY:-toadslop/rtl88x2bu-linux-driver}"
BRANCH="${BRANCH:-master}"

EXPECTED=(
	"Host L2 tests / host-l2"
	"Module L0 build / module-l0"
	"Module L1 symbols / module-l1"
)

if ! command -v gh >/dev/null 2>&1; then
	echo "verify-branch-protection: gh CLI required" >&2
	exit 1
fi

if ! gh api "repos/${REPO}/branches/${BRANCH}/protection" >/tmp/bp.json 2>/dev/null; then
	echo "verify-branch-protection: branch '${BRANCH}' is not protected" >&2
	exit 1
fi

python3 - <<'PY'
import json
import os
import sys

expected = [
    "Host L2 tests / host-l2",
    "Module L0 build / module-l0",
    "Module L1 symbols / module-l1",
]
with open("/tmp/bp.json") as f:
    data = json.load(f)

contexts = set(data.get("required_status_checks", {}).get("contexts") or [])
missing = [c for c in expected if c not in contexts]
extra = sorted(contexts - set(expected))

if missing:
    print("verify-branch-protection: missing required contexts:", file=sys.stderr)
    for c in missing:
        print(f"  - {c}", file=sys.stderr)
    sys.exit(1)

print("verify-branch-protection: OK")
print(f"  branch: {os.environ.get('BRANCH', 'master')}")
print("  required checks:")
for c in expected:
    print(f"    - {c}")
if extra:
    print("  additional contexts on rule (informational):")
    for c in extra:
        print(f"    - {c}")
PY
