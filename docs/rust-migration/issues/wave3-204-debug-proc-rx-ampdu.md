---
title: "[W3-204] Translate rtw_debug_rest.c — proc rx_ampdu get/set"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-204
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port RX AMPDU debug proc handlers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ampdu.rs`](../../../rust/rtw_debug_proc_ampdu.rs):

- `proc_get_rx_ampdu`
- `proc_set_rx_ampdu`

## Notes

- Calls `rtw_rx_ampdu_is_accept` / `rtw_rx_ampdu_size` / `rtw_rx_ampdu_set_*` / `rtw_rx_ampdu_apply` via thin C FFI (adapter helpers stay in C for now).
- Uses existing `dump_regsty_rx_ampdu_size_limit` from [W3-170](wave3-170-debug-regsty-rx-ampdu-limit.md) (`rust/rtw_debug_regsty.rs`).
- Parallel **debug** lane root (new Rust module).

## Acceptance

- L0 + L1 on both handlers
- L2: fixture adapter with fixed/auto accept+size — proc_get output matches C oracle; proc_set updates driver fields
