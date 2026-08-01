---
title: "[W3-58] Translate rtw_rf.c — dump_txpwr_lmt debug formatter"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-58
epic: E05
blocked_by: [W3-57]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `dump_txpwr_lmt`

## Notes

- Deferred from W3-52 (txpwr_lmt CRUD). Reads hal_spec/band caps; medium coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for dump_txpwr_lmt output formatting
