---
title: "[W3-35] Translate rtw_vht.c — VHT MCS and NSS helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-35
epic: E05
blocked_by: [W3-34]
estimate_loc: 200
---

## Goal

Port pure VHT MCS/NSS helpers from [`core/rtw_vht.c`](../../../core/rtw_vht.c) to [`rust/rtw_vht.rs`](../../../rust/rtw_vht.rs):

- `VHT_get_ss_from_map`, `rtw_vht_nss_to_mcsmap`

## Notes

- VHT IE handlers and adapter-coupled paths (`VHT_caps_handler`, `VHTOnAssocRsp`, …) stay in C.
- L2: host harness under `tests/host/vht/` with MCS map vectors.

## Acceptance

- L0 build + L2 host unit tests for VHT MCS/NSS helpers
