---
title: "[W3-21] Translate rtw_rf.c — lookup/format tables"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-21
epic: E05
blocked_by: [W3-20]
estimate_loc: 200
---

## Goal

Port static lookup tables and accessors from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `_ch_width_str`, `_ch_width_to_bw_cap`
- `_band_str`, `_band_to_band_cap`
- `_opc_bw_str`, `_opc_bw_to_ch_width`

## Notes

- Tables move with W3-19/W3-20 into `core/rtw_rf_rest.c`.
- Pure data + pointer-return accessors; no adapter state.
- L2: extend `tests/host/rf/` with table lookup vectors.

## Acceptance

- L0 build + L2 host unit tests for lookup table accessors
