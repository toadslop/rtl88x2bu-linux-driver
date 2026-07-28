---
title: "[W3-33] Translate rtw_rm_util.c — radio measurement pure helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-33
epic: E05
blocked_by: [W3-32]
estimate_loc: 200
---

## Goal

Port pure radio-measurement utility helpers from [`core/rtw_rm_util.c`](../../../core/rtw_rm_util.c) to [`rust/rtw_rm_util.rs`](../../../rust/rtw_rm_util.rs):

- `translate_dbm_to_rcpi`, `translate_percentage_to_rcpi`
- `is_wildcard_bssid`, `rm_get_ch_set`, `rm_get_oper_class_via_ch`

## Notes

- Adapter-coupled RM helpers (`rm_get_tx_power`, `rm_get_bcn_rcpi`, …) stay in C.
- L2: host harness under `tests/host/rm/` with RCPI/ch-set vectors.

## Acceptance

- L0 build + L2 host unit tests for RM util helpers
