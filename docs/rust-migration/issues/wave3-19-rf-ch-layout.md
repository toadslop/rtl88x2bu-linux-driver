---
title: "[W3-19] Translate rtw_rf.c — channel layout helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-19
epic: E05
blocked_by: [W3-18]
estimate_loc: 200
---

## Goal

Port pure channel layout helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `rtw_get_scch_by_cch_offset`, `rtw_get_scch_by_cch_opch`
- `center_chs_2g_num`, `center_chs_2g`, `center_chs_5g_num`, `center_chs_5g`
- `rtw_get_op_chs_by_cch_bw`, `rtw_get_offset_by_chbw`
- `rtw_get_center_ch`, `rtw_get_ch_group`

## Notes

- Extract to `core/rtw_rf_rest.c` (same `*_rest.c` pattern as `rtw_io_rest.c` / `rtw_chplan_rest.c`).
- Static center-ch tables (`center_ch_2g`, `center_ch_5g_*`, `op_chs_of_cch_*`) move with the helpers.
- HAL-dependent RF paths (txpwr, regd_exc, op_class_pref, adapter state) stay in C until later issues.
- L2: host harness under `tests/host/rf/` with synthetic channel/bw/offset vectors.

## Acceptance

- L0 build + L2 host unit tests for channel layout helpers
