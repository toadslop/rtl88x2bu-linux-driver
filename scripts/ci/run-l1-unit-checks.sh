#!/bin/bash
# L1 gate: run per-unit rust-check-symbols-* aggregate targets (T7 PR2).
# Requires KDIR (default /opt/linux) and LLVM=1 via make invocation.
set -euo pipefail

KDIR="${KDIR:-/opt/linux}"
MAKE=(make "KDIR=${KDIR}" LLVM=1)

targets=(
	rust-check-symbols-rtw-chplan
	rust-check-symbols-rtw-swcrypto
	rust-check-symbols-rtw-ieee80211
	rust-check-symbols-rtw-security
	rust-check-symbols-rtw-wlan-util
)

# Optional: space-separated subset from l1-targets-from-diff.sh (T7 PR3).
if [ "$#" -gt 0 ]; then
	read -r -a targets <<<"$*"
fi

for target in "${targets[@]}"; do
	echo "=== L1 unit check: ${target} ==="
	"${MAKE[@]}" "${target}"
done

echo "run-l1-unit-checks: OK (${#targets[@]} target(s))"
