---
title: "[W3-172] Translate rtw_pwrctrl.c — FW PS state poll (DBG)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-172
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port the firmware PS state check from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_fw_ps.rs`](../../../rust/rtw_pwrctrl_fw_ps.rs):

- `rtw_fw_ps_state`

## Notes

- Deferred from closed [W3-167](wave3-167-pwrctrl-ips-enter-leave.md).
- Wrapped in `DBG_CHECK_FW_PS_STATE`; uses `rtw_hal_get_hwreg` / `rtw_hal_set_hwreg` FFI.
- Parallel pwrctrl lane root (disjoint from `rtw_pwrctrl_ips.rs`).

## Acceptance

- L0 + L1 with DBG FW PS gate enabled in test config
- L2 optional: stub HAL `HW_VAR_FW_PS_STATE` table for 8822BU path
