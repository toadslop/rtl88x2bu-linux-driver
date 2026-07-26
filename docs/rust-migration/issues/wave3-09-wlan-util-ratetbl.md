---
title: "[W3-09] Translate rtw_wlan_util.c part 2 — ratetbl + network type"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-09
epic: E05
blocked_by: [W3-08]
estimate_loc: 200
---

## Goal

Port ratetbl conversion and adapter-coupled network-type helpers from [`core/rtw_wlan_util.c`](../../../core/rtw_wlan_util.c):

- `ratetbl_val_2wifirate`
- `is_basicrate`
- `ratetbl2rateset`
- `get_rate_set`, `set_mcs_rate_by_mask`
- `judge_network_type` (reads `padapter->mlmeextpriv` / `mlmext_info` for HT/VHT flags and channel)

## Notes

- `judge_network_type` requires thin FFI to `_adapter` for capability flags; not a pure rate-byte classifier.
- Introduce `NetworkType` domain type (A2).
- L2: ratetbl → rateset vectors and network-type classification from C oracle (T5).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T5)
