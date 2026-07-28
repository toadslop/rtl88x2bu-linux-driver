#!/bin/bash
# Map changed paths to rust-check-symbols-* make targets (T7 PR3).
# Prints a space-separated target list on stdout; full suite on uncertainty.
set -euo pipefail

BASE_REF="${1:-origin/master}"

if ! git rev-parse --verify "${BASE_REF}" >/dev/null 2>&1; then
	echo "l1-targets-from-diff: cannot resolve ${BASE_REF}, using full suite" >&2
	BASE_REF="HEAD~1"
fi

mapfile -t changed < <(
	git diff --name-only "${BASE_REF}"...HEAD 2>/dev/null \
		|| git diff --name-only "${BASE_REF}" HEAD
)

FULL_SUITE=0
declare -A selected=()

add_target() {
	selected["$1"]=1
}

for path in "${changed[@]}"; do
	case "${path}" in
	Makefile \
	| docs/rust-migration/scripts/* \
	| scripts/ci/* \
	| .github/workflows/module-l1.yml)
		FULL_SUITE=1
		break
		;;
	rust/rtw_chplan.rs \
	| core/rtw_chplan.c \
	| core/rtw_chplan.h)
		add_target rust-check-symbols-rtw-chplan
		;;
	rust/rtw_swcrypto.rs \
	| core/rtw_swcrypto.c \
	| core/rtw_swcrypto_rest.c)
		add_target rust-check-symbols-rtw-swcrypto
		;;
	rust/rtw_ieee80211.rs \
	| core/rtw_ieee80211.c \
	| tests/host/ie/*)
		add_target rust-check-symbols-rtw-ieee80211
		;;
	rust/rtw_security.rs \
	| rust/rtw_security_rest.rs \
	| core/rtw_security.c \
	| core/rtw_security_rest.c \
	| tests/host/security/*)
		add_target rust-check-symbols-rtw-security
		add_target rust-check-symbols-rtw-security-rest-misc
		;;
	rust/rtw_wlan_util.rs \
	| core/rtw_wlan_util.c \
	| tests/host/wlan_util/*)
		add_target rust-check-symbols-rtw-wlan-util
		;;
	rust/*)
		FULL_SUITE=1
		break
		;;
	esac
done

ALL_TARGETS=(
	rust-check-symbols-rtw-chplan
	rust-check-symbols-rtw-swcrypto
	rust-check-symbols-rtw-ieee80211
	rust-check-symbols-rtw-security
	rust-check-symbols-rtw-security-rest-misc
	rust-check-symbols-rtw-wlan-util
)

emit() {
	local t
	for t in "$@"; do
		printf '%s ' "${t}"
	done
}

if [ "${FULL_SUITE}" = 1 ] || [ "${#changed[@]}" -eq 0 ] || [ "${#selected[@]}" -eq 0 ]; then
	emit "${ALL_TARGETS[@]}"
	exit 0
fi

for t in "${ALL_TARGETS[@]}"; do
	if [ -n "${selected[$t]+x}" ]; then
		emit "${t}"
	fi
done
