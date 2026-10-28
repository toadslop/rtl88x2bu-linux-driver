---
title: "[W3-187] Translate rtw_pwrctrl.c — set PS mode (part 2/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-187
epic: E05
blocked_by: [W3-186]
estimate_loc: 220
---

## Goal

Complete `rtw_set_ps_mode` in [`rust/rtw_pwrctrl_set_ps.rs`](../../../rust/rtw_pwrctrl_set_ps.rs) (continued from [W3-186](wave3-186-pwrctrl-set-ps-mode-p1.md)):

- LPS enter H2C / `rtw_exec_lps` integration
- Smart PS / bcn_ant_mode updates and `#ifdef CONFIG_LPS_LCLK` lock pairing

## Notes

- Depends on [W3-181](wave3-181-pwrctrl-exec-lps-rfon.md) / `rtw_exec_lps` Rust symbol on stack base when implementing.
- Makefile drops remaining C body for `rtw_set_ps_mode`.

## Acceptance

- L0 + L1 full symbol swap on `rtw_set_ps_mode` (deferred from W3-89-style part 1)
- L2: differential enter/leave LPS mode with stub HAL H2C capture
