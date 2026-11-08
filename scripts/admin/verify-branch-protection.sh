#!/bin/bash
# T18/T19: Read-only check that master requires L0/L1/L2 + rustfmt + module-l3 (#1795, #1796).
set -euo pipefail

REPO="${GITHUB_REPOSITORY:-toadslop/rtl88x2bu-linux-driver}"
BRANCH="${BRANCH:-master}"

EXPECTED=(
	host-l2
	module-l0
	module-l1
	rustfmt
	module-l3
)

if ! command -v gh >/dev/null 2>&1; then
	echo "verify-branch-protection: gh CLI required" >&2
	exit 1
fi

if ! gh api "repos/${REPO}/branches/${BRANCH}/protection" >/tmp/bp.json 2>/dev/null; then
	echo "verify-branch-protection: branch '${BRANCH}' is not protected" >&2
	exit 1
fi

EXPECTED_CONTEXTS="$(printf '%s\n' "${EXPECTED[@]}")" python3 - <<'PY'
import json
import os
import sys

expected = [c for c in os.environ["EXPECTED_CONTEXTS"].split("\n") if c]
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
print("  required check contexts:")
for c in expected:
    print(f"    - {c}")
if extra:
    print("  additional contexts on rule (informational):")
    for c in extra:
        print(f"    - {c}")
PY
