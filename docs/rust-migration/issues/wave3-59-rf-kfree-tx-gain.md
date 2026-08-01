---
title: "[W3-59] Translate rtw_rf.c — kfree TX gain offset apply"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-59
epic: E05
blocked_by: [W3-58]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `rtw_rf_get_kfree_tx_gain_offset`
- `rtw_rf_set_tx_gain_offset`
- `rtw_rf_apply_tx_gain_offset`

## Notes

- High HAL coupling (rtw_hal_read/write_rfreg). Expect thin C shims at FFI edge.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L1 symbols + L2 host unit tests for kfree TX gain offset helpers
