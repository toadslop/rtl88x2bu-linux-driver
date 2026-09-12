#!/usr/bin/env bash
# Verify every rust/** definition of _SUCCESS / _FAIL matches include/osdep_service.h.
#
# These constants are ABI, not an internal choice: C callers that still hold the
# call site (for example core/rtw_recv.c around recv_decache, wlanhdr_to_ethhdr
# and rtw_tkip_decrypt) compare the value a Rust port returns against the kernel
# macros. A port that reuses the host-harness convention (0 == success) inverts
# drop/accept decisions in the RX path, so pin the spelling here.
#
# Host-only conventions are fine as long as they use a distinct name
# (for example HOST_GCMP_SUCCESS in rust/rtw_security_rest.rs).
set -euo pipefail

osdep="include/osdep_service.h"

expect_success=$(sed -nE 's/^#define[[:space:]]+_SUCCESS[[:space:]]+([0-9-]+).*/\1/p' "${osdep}")
expect_fail=$(sed -nE 's/^#define[[:space:]]+_FAIL[[:space:]]+([0-9-]+).*/\1/p' "${osdep}")

if [ -z "${expect_success}" ] || [ -z "${expect_fail}" ]; then
	echo "check-status-constants: could not read _SUCCESS/_FAIL from ${osdep}" >&2
	exit 1
fi

rc=0
found=0

while IFS=: read -r file line text; do
	name=$(printf '%s' "${text}" | sed -nE 's/.*const[[:space:]]+(_SUCCESS|_FAIL).*/\1/p')
	value=$(printf '%s' "${text}" | sed -nE 's/.*const[[:space:]]+(_SUCCESS|_FAIL)[[:space:]]*:[^=]*=[[:space:]]*(-?[0-9]+).*/\2/p')
	[ -z "${name}" ] && continue
	found=$((found + 1))

	if [ "${name}" = "_SUCCESS" ]; then
		want="${expect_success}"
	else
		want="${expect_fail}"
	fi

	if [ "${value}" != "${want}" ]; then
		echo "${file}:${line}: ${name} is ${value}, kernel ${osdep} says ${want}" >&2
		rc=1
	fi
done < <(grep -rnE '^[[:space:]]*(pub )?const[[:space:]]+(_SUCCESS|_FAIL)[[:space:]]*:' rust --include='*.rs')

if [ "${found}" -eq 0 ]; then
	echo "check-status-constants: no _SUCCESS/_FAIL definitions found under rust/" >&2
	exit 1
fi

if [ "${rc}" -ne 0 ]; then
	echo >&2
	echo "check-status-constants: Rust ports must return the kernel status values." >&2
	echo "If a host harness needs a different convention, give it a distinct name" >&2
	echo "(see HOST_GCMP_SUCCESS in rust/rtw_security_rest.rs)." >&2
	exit 1
fi

echo "check-status-constants: OK (${found} definitions match ${osdep})"
