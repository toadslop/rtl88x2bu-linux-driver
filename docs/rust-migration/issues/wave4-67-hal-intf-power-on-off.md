---
title: "[W4-67] Translate hal_intf.c — hal_power_on + hal_power_off"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-67
epic: E06
blocked_by: [W4-63]
estimate_loc: 100
---

## Goal

Port power wrapper entry points from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_power_on`
- `rtw_hal_power_off`

## Notes

- Parallel hal_intf lane root (disjoint symbols from W4-64 RF path chain); shares `rust/hal_intf.rs` but does not depend on rfpath/trxnss init.
- `CONFIG_BT_COEXIST` hooks call existing bt coexist C helpers through FFI; clear `macid_ctl` arrays in `rtw_hal_power_off` via shims matching C offsets.
- `CONFIG_LPS_1T1R` reset of `lps_1t1r` must match C.

## Acceptance

- L0 + L1 symbol parity for both functions
- L3 smoke optional: no new WARN on module load/unload in QEMU
