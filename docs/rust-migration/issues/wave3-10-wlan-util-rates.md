---
title: "[W3-10] Translate rtw_wlan_util.c part 1 — rate classification"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-10
epic: E05
blocked_by: [A2]
estimate_loc: 200
---

## Goal

Port pure rate-classification helpers from [`core/rtw_wlan_util.c`](../../../core/rtw_wlan_util.c):

- `rtw_is_cck_rate`, `rtw_is_ofdm_rate`
- `rtw_is_basic_rate_cck`, `rtw_is_basic_rate_ofdm`, `rtw_is_basic_rate_mix`
- `cckrates_included`, `cckratesonly_included`
- `judge_network_type`

Begin multi-part port of the 5664-line file; extract remainder to `rtw_wlan_util_rest.c`.

## Notes

- Introduce `WifiRate` / `NetworkType` domain types (A2).
- L2: exhaustive rate-byte classification table (802.11b/g/n legacy rates).

## Acceptance

- L0 build + L1 symbols + L2 host vectors
