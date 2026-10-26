---
title: "[W3-181] Translate rtw_pwrctrl.c — exec LPS and RF-on control"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-181
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port LPS execution helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_lps_exec.rs`](../../../rust/rtw_pwrctrl_lps_exec.rs):

- `rtw_exec_lps`
- `rtw_lps_rfon_ctrl`

## Notes

- HAL via `HW_VAR_H2C_FW_PWRMODE`, `HW_VAR_LPS_STATE_CHK`, `HW_VAR_SET_RPWM` FFI — no MMIO in Rust.
- `CONFIG_LPS_ACK` mutex/sctx paths must mirror C ordering.
- Parallel pwrctrl lane (disjoint from `rtw_pwrctrl_alive.rs`).

## Acceptance

- L0 + L1 on both symbols
- L2: stub adapter + `MLME_IS_ASOC` gate — verify H2C payload bytes for active vs LPS modes
