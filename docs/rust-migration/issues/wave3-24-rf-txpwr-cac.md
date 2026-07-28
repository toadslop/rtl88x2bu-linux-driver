---
title: "[W3-24] Translate rtw_rf.c — txpwr format and DFS CAC helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-24
epic: E05
blocked_by: [W3-23]
estimate_loc: 200
---

## Goal

Port txpwr formatting and DFS CAC helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `txpwr_idx_get_dbm_str`, `txpwr_mbm_get_dbm_str`, `mb_of_ntx`
- `rtw_is_long_cac_range`, `rtw_is_long_cac_ch`

## Notes

- Depends on W3-20 (`rtw_chbw_to_freq_range` for CAC range checks).
- Adapter-dependent txpwr (`rtw_rf_get_kfree_tx_gain_offset`, `rtw_txpwr_lmt_*`) stays in C.
- L2: host harness for dbm string formatting and CAC range vectors.

## Acceptance

- L0 build + L2 host unit tests for txpwr format and CAC helpers
