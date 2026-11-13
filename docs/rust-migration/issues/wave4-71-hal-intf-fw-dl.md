---
title: "[W4-71] Translate hal_intf.c — hal_fw_dl"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-71
epic: E06
blocked_by: [W4-70]
estimate_loc: 80
---

## Goal

Port `rtw_hal_fw_dl` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs).

## Notes

- Wraps `padapter->hal_func.fw_dl` and clears `lps_1t1r` when `CONFIG_LPS_1T1R` — use HAL data field shims already used in W4-67.
- Return type `s32` and wowlan flag pass-through must match C exactly.

## Acceptance

- L0 + L1 symbol parity for `rtw_hal_fw_dl`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-70
