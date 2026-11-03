---
title: "[W3-207] Translate rtw_debug_rest.c — proc tx AMSDU debug leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-207
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port TX AMSDU debug proc handlers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_amsdu.rs`](../../../rust/rtw_debug_proc_amsdu.rs) (`CONFIG_TX_AMSDU`):

- `proc_get_tx_amsdu` / `proc_set_tx_amsdu`
- `proc_get_tx_amsdu_rate` / `proc_set_tx_amsdu_rate`

## Notes

- `proc_set_tx_amsdu` value `3` clears `xmitpriv` AMSDU debug counters — preserve semantics.
- Parallel **debug** lane (separate Rust module from AMPDU proc work).

## Acceptance

- L0 with `CONFIG_TX_AMSDU`
- L1 on all four handlers
- L2: fixture — counter reset path + rate setter golden output
