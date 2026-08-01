---
title: "[W3-52] Translate rtw_rf.c — txpwr_lmt list CRUD"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-52
epic: E05
blocked_by: [W3-51]
estimate_loc: 200
---

## Goal

Port tx power limit list helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `_rtw_txpwr_lmt_get_by_name`, `rtw_txpwr_lmt_get_by_name`
- `rtw_txpwr_lmt_add_with_nlen`, `rtw_txpwr_lmt_add`, `rtw_txpwr_lmt_list_free`

## Notes

- Large `txpwr_lmt_ent` structures; needs `hal_spec` init values in L2 fixtures.
- `dump_txpwr_lmt` and kfree tx gain offset helpers stay in C.
- L2: extend `tests/host/rf/` with limit entry min/max merge vectors.

## Acceptance

- L0 build + L2 host unit tests for txpwr_lmt list CRUD
