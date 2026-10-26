---
title: "[W3-175] Translate rtw_pwrctrl.c — RPWM/CPWM LPS clock"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-175
epic: E05
blocked_by: [W3-172]
estimate_loc: 200
---

## Goal

Port LPS 32k clock RPWM/CPWM helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_rpwm.rs`](../../../rust/rtw_pwrctrl_rpwm.rs):

- `rtw_cpwm_polling`
- `rtw_set_rpwm`

## Notes

- Fills the RPWM/CPWM gap called out in [W3-94](wave3-94-pwrctrl-ps-gate-lps.md).
- `rtw_cpwm_polling` may call `rtw_fw_ps_state` under `DBG_CHECK_FW_PS_STATE` — land [W3-172](wave3-172-pwrctrl-fw-ps-state.md) first.
- Heavy `#ifdef CONFIG_LPS_LCLK` / WOWLAN branches — match C guards; HAL via `HW_VAR_SET_RPWM` / `HW_VAR_CPWM` FFI.

## Acceptance

- L0 + L1 on swapped symbols with LPS LCLK config used in CI
- L2 optional: stub HAL CPWM toggle table for polling success path
