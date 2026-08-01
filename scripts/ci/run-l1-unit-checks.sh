#!/bin/bash
# L1 gate: run per-unit rust-check-symbols-* aggregate targets (T7 PR2).
# Requires KDIR (default /opt/linux) and LLVM=1 via make invocation.
set -euo pipefail

KDIR="${KDIR:-/opt/linux}"
MAKE=(make "KDIR=${KDIR}" LLVM=1)

targets=(
	rust-check-symbols-rtw-chplan
	rust-check-symbols-rtw-chplan-rest
	rust-check-symbols-rtw-io-rest
	rust-check-symbols-rtw-swcrypto
	rust-check-symbols-rtw-ieee80211
	rust-check-symbols-rtw-ieee80211-rest
	rust-check-symbols-rtw-security
	rust-check-symbols-rtw-security-rest-misc
	rust-check-symbols-rtw-wlan-util
	rust-check-symbols-rtw-rf-rest
)

# Optional: space-separated subset from l1-targets-from-diff.sh (T7 PR3).
if [ "$#" -gt 0 ]; then
	read -r -a targets <<<"$*"
	if [ "${#targets[@]}" -eq 0 ] || [ -z "${targets[0]:-}" ]; then
		echo "run-l1-unit-checks: no targets in scope — skip"
		exit 0
	fi
fi

for target in "${targets[@]}"; do
	echo "=== L1 unit check: ${target} ==="
	"${MAKE[@]}" "${target}"
done

echo "run-l1-unit-checks: OK (${#targets[@]} target(s))"
