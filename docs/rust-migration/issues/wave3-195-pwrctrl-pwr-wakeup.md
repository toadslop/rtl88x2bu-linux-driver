---
title: "[W3-195] Translate rtw_pwrctrl.c — IPS/USB pwr wakeup core"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-195
epic: E05
blocked_by: [W3-194]
estimate_loc: 220
---

## Goal

Port the primary NIC wakeup path from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_wakeup.rs`](../../../rust/rtw_pwrctrl_wakeup.rs):

- `_rtw_pwr_wakeup` and public `rtw_pwr_wakeup` wrapper
- `rtw_set_ips_deny` (inline timer bump used by wakeup and IPS paths)

## Notes

- Depends on early-suspend wait helpers from [W3-194](wave3-194-pwrctrl-early-suspend-resume.md) (`rtw_is_do_late_resume`, `bInSuspend` polling).
- Calls `LeaveAllPowerSaveMode`, `ips_leave`, and MLME/HW state checks via existing C/Rust symbols — no new HAL surface.
- Unblocks full L2 coverage for [W3-184](wave3-184-pwrctrl-pm-setters.md) `rtw_pm_set_ips` IPS_NONE → wakeup path (today uses C `rtw_pwr_wakeup` stub).

## Acceptance

- L0 + L1 on wakeup + ips_deny symbols
- L2: host stub adapter — associated vs unassociated, rf_off → ips_leave, suspend wait timeout paths
