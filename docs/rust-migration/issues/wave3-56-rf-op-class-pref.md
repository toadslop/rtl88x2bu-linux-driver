---
title: "[W3-56] Translate rtw_rf.c — op_class_pref lifecycle and regulatory apply"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-56
epic: E05
blocked_by: [W3-55]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `op_class_pref_init`
- `op_class_pref_deinit`
- `op_class_pref_apply_regulatory`

## Notes

- Uses hal_chk_band_cap, GET_HAL_SPEC, chset + W3-22 op-class helpers. Medium HAL coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for op_class_pref lifecycle (adapter/chset fixtures documented in harness)
