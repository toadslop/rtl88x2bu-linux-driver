---
title: "[W3-174] Translate rtw_pwrctrl.c — leave LPS traffic check (TP)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-174
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port the CONFIG_LPS_CHK_BY_TP leave-LPS helper from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_lps_tp.rs`](../../../rust/rtw_pwrctrl_lps_tp.rs):

- `traffic_check_for_leave_lps_by_tp`

## Notes

- Gated on `CONFIG_LPS`, `CONFIG_CHECK_LEAVE_LPS`, `CONFIG_LPS_CHK_BY_TP` — document which are on for 8822BU CI.
- Pure arithmetic over `sta_stats` byte counters + `pwrctrl_priv` thresholds; may call `LPS_Leave` via existing Rust/C symbol.
- Parallel pwrctrl lane (disjoint module file).

## Acceptance

- L0 + L1 when LPS TP check config is enabled
- L2: JSON vectors for threshold / counter combinations → leave_lps decision
