---
title: "[W3-23] Translate rtw_rf.c — RF type and trx path helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-23
epic: E05
blocked_by: [W3-22]
estimate_loc: 200
---

## Goal

Port RF type / trx-path bitmap helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `rf_type_to_default_trx_bmp`, `trx_num_to_rf_type`, `trx_bmp_to_rf_type`
- `rf_type_is_a_in_b`
- `rtw_restrict_trx_path_bmp_by_trx_num_lmt`, `rtw_restrict_trx_path_bmp_by_rftype`

## Notes

- `_trx_num_to_rf_type` table moves with helpers.
- `tx_path_nss_set_*` deferred to W3-25.
- L2: host harness with rf_type / trx_bmp vectors.

## Acceptance

- L0 build + L2 host unit tests for RF type / trx path helpers
