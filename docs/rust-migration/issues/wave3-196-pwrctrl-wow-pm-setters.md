---
title: "[W3-196] Translate rtw_pwrctrl.c — WoWLAN PM ioctl setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-196
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port `CONFIG_WOWLAN` proc/ioctl PM setters from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) into
[`rust/rtw_pwrctrl_pm.rs`](../../../rust/rtw_pwrctrl_pm.rs) (extend existing module):

- `rtw_pm_set_wow_lps`
- `rtw_pm_set_wow_lps_level`
- `rtw_pm_set_wow_lps_1t1r` when `CONFIG_LPS_1T1R`

## Notes

- Parallel pwrctrl lane — registry field writes only; disjoint from in-flight init ([W3-190](wave3-190-pwrctrl-init-free-p2.md)) and wakeup ([W3-195](wave3-195-pwrctrl-pwr-wakeup.md)).
- Mirror `-EINVAL` handling from [W3-184](wave3-184-pwrctrl-pm-setters.md).

## Acceptance

- L0 + L1 on all WoWLAN PM setter symbols (under `CONFIG_WOWLAN`)
- L2: stub `pwrctrl_priv` — valid mode/level updates and invalid enum rejection
