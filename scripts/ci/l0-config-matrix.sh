#!/bin/bash
# T22: build 88x2bu.ko under non-default Makefile configs (advisory matrix).
# Pass = modpost links without undefined symbols and the .ko is produced.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "${ROOT}"

KDIR="${KDIR:-/opt/linux}"
JOBS="${JOBS:-$(nproc)}"
export LIBCLANG_PATH="${LIBCLANG_PATH:-/usr/lib/llvm-18/lib}"
MAKE=(make KDIR="${KDIR}" LLVM=1 -j"${JOBS}")

# id|make overrides (eval as make CLI)|expect: pass|known_fail|fx|note
ENTRIES=(
  'wowlan-gpio|CONFIG_WOWLAN=y CONFIG_GPIO_WAKEUP=y|pass||extra WoWLAN + GPIO wakeup'
  'power-saving-off|CONFIG_POWER_SAVING=n|pass||pwrctrl Rust objects gated off C side'
  'tx-amsdu|USER_EXTRA_CFLAGS=-DCONFIG_TX_AMSDU|pass||TX AMSDU via ccflags (FX-07 row 6)'
  'proc-debug-off|CONFIG_PROC_DEBUG=n|pass||debug proc Rust TU omitted'
  'rtw-80211k|CONFIG_RTW_80211K=y|pass||802.11k enabled'
  'tdls|CONFIG_TDLS=y|known_fail|FX-07|Clang -Wparentheses-equality in rtw_mlme_ext.c'
  'ap-wowlan-no-ap|CONFIG_AP_WOWLAN=y CONFIG_AP_MODE=n CONFIG_P2P=n|known_fail|FX-07|cfg / Rust object parity (Makefile row 1)'
  'gpio-wakeup-no-wowlan|CONFIG_GPIO_WAKEUP=y CONFIG_WOWLAN=n|known_fail|FX-07|GPIO wakeup without WoWLAN (row 7)'
)

pass_ok=0
pass_fail=0
known_fail_ok=0
known_fail_xfail=0

build_one() {
  local id="$1"
  local make_args="$2"
  local log
  log="$(mktemp)"
  make clean >/dev/null 2>&1
  # shellcheck disable=SC2086
  if "${MAKE[@]}" ${make_args} >"${log}" 2>&1 && test -f "${ROOT}/88x2bu.ko"; then
    rm -f "${log}"
    return 0
  fi
  if test -f "${log}"; then
    rg -i "undefined reference|modpost.*undefined|error:" "${log}" | head -8 >&2 || true
    tail -20 "${log}" >&2
  fi
  rm -f "${log}"
  return 1
}

echo "l0-config-matrix: KDIR=${KDIR} (default 8822BU USB product config)"

for row in "${ENTRIES[@]}"; do
  IFS='|' read -r id make_args expect fx note <<<"${row}"
  if build_one "${id}" "${make_args}"; then
    got=pass
  else
    got=fail
  fi

  case "${expect}:${got}" in
    pass:pass)
      printf '  %-24s PASS  (%s)\n' "${id}" "${note}"
      pass_ok=$((pass_ok + 1))
      ;;
    pass:fail)
      printf '  %-24s FAIL  (expected pass) — %s\n' "${id}" "${note}" >&2
      pass_fail=$((pass_fail + 1))
      ;;
    known_fail:fail)
      printf '  %-24s XFAIL (%s) — fix: %s\n' "${id}" "${note}" "${fx:-TBD}"
      known_fail_ok=$((known_fail_ok + 1))
      ;;
    known_fail:pass)
      printf '  %-24s UNEXPECTED PASS — %s (drop known_fail?)\n' "${id}" "${note}"
      known_fail_xfail=$((known_fail_xfail + 1))
      ;;
  esac
done

echo "l0-config-matrix summary: ${pass_ok} passed, ${pass_fail} unexpected fails," \
     "${known_fail_ok} expected fails, ${known_fail_xfail} stale xfail markers"

if [ "${pass_fail}" -gt 0 ] || [ "${known_fail_xfail}" -gt 0 ]; then
  echo "l0-config-matrix: blocking failure (advisory job still reports this)" >&2
  exit 1
fi

echo "l0-config-matrix: OK (advisory — known FX entries still fail until landed)"
exit 0
