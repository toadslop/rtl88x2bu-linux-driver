#!/bin/bash
# T18: Enable branch protection on master with L0/L1/L2 required checks.
# Requires a token with repo admin rights (Settings → Branches API).
# Automation PATs without administration scope return HTTP 403 — run locally as maintainer.
set -euo pipefail

REPO="${GITHUB_REPOSITORY:-toadslop/rtl88x2bu-linux-driver}"
BRANCH="${BRANCH:-master}"

# Literal check-run context strings (job `name:` in workflows). Branch protection
# matches these — not the "Workflow name / job id" label GitHub may show in the UI.
REQUIRED_CONTEXTS=(
	host-l2
	module-l0
	module-l1
)

if ! command -v gh >/dev/null 2>&1; then
	echo "enable-branch-protection: gh CLI required" >&2
	exit 1
fi

payload="$(CONTEXTS="$(printf '%s\n' "${REQUIRED_CONTEXTS[@]}")" python3 - <<'PY'
import json
import os

contexts = [c for c in os.environ["CONTEXTS"].split("\n") if c]
print(
    json.dumps(
        {
            "required_status_checks": {"strict": True, "contexts": contexts},
            "enforce_admins": False,
            "required_pull_request_reviews": {
                "dismiss_stale_reviews": False,
                "require_code_owner_reviews": False,
                "require_last_push_approval": False,
                "required_approving_review_count": 0,
            },
            "restrictions": None,
            "allow_force_pushes": False,
            "allow_deletions": False,
            "block_creations": False,
            "required_conversation_resolution": False,
            "lock_branch": False,
            "allow_fork_syncing": True,
        }
    )
)
PY
)"

echo "enable-branch-protection: applying rule on ${REPO} branch ${BRANCH}"
echo "  required check contexts:"
printf '    - %s\n' "${REQUIRED_CONTEXTS[@]}"

gh api --method PUT "repos/${REPO}/branches/${BRANCH}/protection" --input - <<<"${payload}"

echo "enable-branch-protection: OK — run scripts/admin/verify-branch-protection.sh to confirm"
