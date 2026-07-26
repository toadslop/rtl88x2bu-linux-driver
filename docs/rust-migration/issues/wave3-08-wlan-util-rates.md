---
title: "[W3-08] Translate rtw_wlan_util.c part 1 — pure rate classification"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-08
epic: E05
blocked_by: [A2, T5]
estimate_loc: 200
---

## Goal

Port **pure** rate-classification helpers from [`core/rtw_wlan_util.c`](../../../core/rtw_wlan_util.c):

- `rtw_is_cck_rate`, `rtw_is_ofdm_rate`
- `rtw_is_basic_rate_cck`, `rtw_is_basic_rate_ofdm`, `rtw_is_basic_rate_mix`
- `cckrates_included`, `cckratesonly_included`

Begin multi-part port of the 5664-line file; extract remainder to `rtw_wlan_util_rest.c`.

## Notes

- `judge_network_type` is **not** in this slice — it reads `_adapter` / `mlmeextpriv` and moves to W3-09.
- Introduce `WifiRate` domain type (A2).
- L2: exhaustive rate-byte classification table (T5 harness).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T5)
