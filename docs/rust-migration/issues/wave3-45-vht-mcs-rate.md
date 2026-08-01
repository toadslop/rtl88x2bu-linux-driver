---
title: "[W3-45] Translate rtw_vht.c — VHT MCS and rate pure helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-45
epic: E05
blocked_by: [W3-44]
estimate_loc: 200
---

## Goal

Port VHT MCS/rate pure helpers from [`core/rtw_vht.c`](../../../core/rtw_vht.c) to [`rust/rtw_vht.rs`](../../../rust/rtw_vht.rs):

- `rtw_get_vht_highest_rate`, `rtw_vht_mcsmap_to_nss`, `rtw_vht_mcs_to_data_rate`
- `rtw_vht_mcs_map_to_bitmap`, `rtw_check_for_vht20`

## Notes

- Builds on W3-35 MCS map helpers; no adapter/assoc state.
- VHT IE build/attach and mgmt handlers stay in C (W3-36 covered restructure).
- L2: new or extended `tests/host/vht/` vectors for MCS→rate/NSS/bitmap.

## Acceptance

- L0 build + L2 host unit tests for VHT MCS and rate pure helpers
