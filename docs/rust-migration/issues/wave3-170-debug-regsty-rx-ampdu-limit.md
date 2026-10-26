---
title: "[W3-170] Translate rtw_debug_rest.c — regsty RX AMPDU size limit dump"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-170
epic: E05
blocked_by: []
estimate_loc: 80
---

## Goal

Port the registry RX AMPDU limit table formatter from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) to
[`rust/rtw_debug_regsty.rs`](../../../rust/rtw_debug_regsty.rs):

- `dump_regsty_rx_ampdu_size_limit`

## Notes

- Pure formatting over `registry_priv.rx_ampdu_sz_limit_by_nss_bw` — no HAL/MMIO.
- Called from `proc_get_rx_ampdu` in C; keep proc handler thin.
- Parallel to [W3-169](wave3-169-debug-rf-bb-reg-dump-ex.md) (disjoint Rust module).

## Acceptance

- L0 + L1 on swapped symbol
- L2: host fixture with a fixed `registry_priv` table → golden print lines
