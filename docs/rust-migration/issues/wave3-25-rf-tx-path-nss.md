---
title: "[W3-25] Translate rtw_rf.c — tx path NSS and bb gain sel"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-25
epic: E05
blocked_by: [W3-24]
estimate_loc: 200
---

## Goal

Port remaining pure RF helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `tx_path_nss_set_default`, `tx_path_nss_set_full_tx`
- `rtw_ch_to_bb_gain_sel`

## Notes

- Completes the leaf `rtw_rf_rest.c` slice; adapter-coupled txpwr/regd_exc/op_class_pref remain in C.
- L2: host harness for NSS path assignment and bb gain sel vectors.

## Acceptance

- L0 build + L2 host unit tests
