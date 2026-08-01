---
title: "[W3-94] ps deny gate and LPS enter/leave"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-94
epic: E05
blocked_by: [W3-93]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to [`rust/rtw_pwrctrl.rs`](../../../rust/rtw_pwrctrl.rs):

- `rtw_ps_deny`
- `rtw_ps_deny_cancel`
- `rtw_ps_deny_get`
- `rtw_pwr_unassociated_idle`
- `LPS_Enter`
- `LPS_Leave`
- `LeaveAllPowerSaveMode`

## Notes

- First pwrctrl tranche; IPS/rpwm/cpwm deferred to a later issue.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for ps deny gate and LPS enter/leave
