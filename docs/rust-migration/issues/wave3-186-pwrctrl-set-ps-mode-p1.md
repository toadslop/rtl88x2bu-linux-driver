---
title: "[W3-186] Translate rtw_pwrctrl.c — set PS mode (part 1/2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-186
epic: E05
blocked_by: []
estimate_loc: 220
---

## Goal

Port the first half of `rtw_set_ps_mode` from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_set_ps.rs`](../../../rust/rtw_pwrctrl_set_ps.rs):

- Early validation, `#ifdef CONFIG_FW_MULTI_PORT_SUPPORT` rsvd page hook
- `#ifdef CONFIG_LPS_PG` PG handle block (enter path preamble)
- Full **active-mode leave** branch (`ps_mode == PS_MODE_ACTIVE`), including WOWLAN 32k wait,
  LPS_PG handles, `HW_VAR_H2C_INACTIVE_IPS`, and `rtw_exec_lps()` (~1616–1621 in C)

Stop before the enter-LPS `else` branch (~1641).

## Notes

- Active leave still calls `rtw_exec_lps`; stack on [W3-181](wave3-181-pwrctrl-exec-lps-rfon.md) Rust export
  (or wait for that stack to merge) before wiring HAL side effects in Rust.
- Part 2 ([W3-187](wave3-187-pwrctrl-set-ps-mode-p2.md)) covers enter-LPS, smart PS updates, and lock exit.
- Same-file lane; split follows W3-89/W3-90: PR1 is C-oracle + L2 only; full L1 on `rtw_set_ps_mode` lands in part 2.

## Acceptance

- L0 build + L2 host unit tests for early-return and active-mode leave (C oracle retains `rtw_set_ps_mode` export)
