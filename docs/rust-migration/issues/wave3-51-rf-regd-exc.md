---
title: "[W3-51] Translate rtw_rf.c — regd_exc list CRUD and search"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-51
epic: E05
blocked_by: [W3-50]
estimate_loc: 200
---

## Goal

Port regulatory exception list helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `_rtw_regd_exc_search`, `rtw_regd_exc_search`, `rtw_regd_exc_add_with_nlen`, `rtw_regd_exc_add`
- `rtw_regd_exc_list_free`, `_dump_regd_exc_list`, `dump_regd_exc_list`

## Notes

- W3-19…W3-25 covered ch layout, freq, op-class, trx path, txpwr fmt; this is the regd_exc remainder.
- List + mutex coupling on `rf_ctl_t`; document lock ordering at FFI edge.
- `op_class_pref_*` and `dump_txpwr_lmt` deferred to a later slice.
- L2: extend `tests/host/rf/` with add/search/free on populated rf_ctl fixtures.

## Acceptance

- L0 build + L2 host unit tests for regd_exc list CRUD and search
