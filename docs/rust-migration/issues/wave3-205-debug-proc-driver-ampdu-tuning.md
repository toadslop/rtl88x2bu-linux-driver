---
title: "[W3-205] Translate rtw_debug_rest.c — proc driver AMPDU tuning leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-205
epic: E05
blocked_by: [W3-204]
estimate_loc: 200
---

## Goal

Port driver AMPDU spacing/factor debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ampdu.rs`](../../../rust/rtw_debug_proc_ampdu.rs):

- `proc_get_rx_ampdu_factor` / `proc_set_rx_ampdu_factor`
- `proc_get_rx_ampdu_density` / `proc_set_rx_ampdu_density`
- `proc_get_tx_ampdu_density` / `proc_set_tx_ampdu_density`
- `proc_get_tx_max_agg_num` / `proc_set_tx_max_agg_num`

## Notes

- Same Rust module as [W3-204](wave3-204-debug-proc-rx-ampdu.md) — chain after rx_ampdu slice lands.
- Adapter field clamping (0xFF sentinel) must match C.

## Acceptance

- L0 + L1 on all listed symbols
- L2: golden proc_get lines + proc_set clamp behavior for out-of-range inputs
