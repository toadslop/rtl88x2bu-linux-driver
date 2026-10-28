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
- `#ifdef CONFIG_LPS_PG` PG handle block
- Active-mode leave path through TDLS sta loop (stop before H2C FW power mode issue)

## Notes

- Split at a natural H2C boundary; part 2 completes enter-LPS and timer/workitem side effects.
- Same-file lane; part 2 chains on this issue.

## Acceptance

- L0 + L1 partial object swap (C stub calls into Rust for ported region)
- L2: host vectors for early-return and active-mode leave counter bumps
