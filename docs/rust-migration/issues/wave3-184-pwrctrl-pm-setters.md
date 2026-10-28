---
title: "[W3-184] Translate rtw_pwrctrl.c — PM ioctl setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-184
epic: E05
blocked_by: []
estimate_loc: 100
---

## Goal

Port proc/ioctl-facing PM mode setters from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_pm.rs`](../../../rust/rtw_pwrctrl_pm.rs):

- `rtw_pm_set_ips`
- `rtw_pm_set_lps`
- `rtw_pm_set_lps_level`

## Notes

- Thin wrappers around `rtw_ips_mode_req`, `rtw_pwr_wakeup`, registry-backed LPS level fields.
- Parallel lane (disjoint module); no dependency on in-flight [W3-182](wave3-182-pwrctrl-wow-lps-level.md).

## Acceptance

- L0 + L1 on all three symbols
- L2: stub adapter — IPS mode transitions and invalid `-EINVAL` paths
