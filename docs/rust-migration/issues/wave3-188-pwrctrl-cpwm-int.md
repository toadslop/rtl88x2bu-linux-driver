---
title: "[W3-188] Translate rtw_pwrctrl.c — CPWM int and leave check"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-188
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port LPS 32k CPWM interrupt path from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_cpwm_int.rs`](../../../rust/rtw_pwrctrl_cpwm_int.rs):

- `LPS_Leave_check`
- `cpwm_int_hdl`

## Notes

- `#ifdef CONFIG_LPS_LCLK` only; uses pwrctrl lock + yield/msleep like C.
- Complements [W3-175](wave3-175-pwrctrl-rpwm-cpwm.md) RPWM setters — disjoint module file.

## Acceptance

- L0 + L1 under LPS LCLK config
- L2 optional: synthetic reportpwrstate + timeout branch in host harness
