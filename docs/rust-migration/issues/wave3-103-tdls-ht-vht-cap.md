---
title: "[W3-103] tdls HT/VHT cap process"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-103
epic: E05
blocked_by: [W3-102]
estimate_loc: 215
---

## Goal

Port helpers from [`core/rtw_tdls.c`](../../../core/rtw_tdls.c) to [`rust/rtw_tdls.rs`](../../../rust/rtw_tdls.rs):

- `rtw_tdls_process_ht_cap`
- `rtw_tdls_process_vht_cap`
- `rtw_tdls_process_vht_operation`
- `rtw_tdls_process_vht_op_mode_notify`

## Notes

- TDLS HT/VHT capability processing; medium adapter coupling via stainfo fields.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for tdls HT/VHT cap process helpers
