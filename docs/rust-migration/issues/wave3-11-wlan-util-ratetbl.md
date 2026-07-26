---
title: "[W3-11] Translate rtw_wlan_util.c part 2 — ratetbl helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-11
epic: E05
blocked_by: [W3-10]
estimate_loc: 200
---

## Goal

Port ratetbl conversion helpers from [`core/rtw_wlan_util.c`](../../../core/rtw_wlan_util.c):

- `ratetbl_val_2wifirate`
- `is_basicrate`
- `ratetbl2rateset`
- `get_rate_set`, `set_mcs_rate_by_mask`

## Notes

- Some functions take `_adapter *` for capability checks — thin FFI where needed.
- L2: ratetbl → rateset vectors from C oracle.

## Acceptance

- L0 build + L1 symbols + L2 host vectors
