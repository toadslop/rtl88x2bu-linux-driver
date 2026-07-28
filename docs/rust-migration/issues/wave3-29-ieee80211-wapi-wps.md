---
title: "[W3-29] Translate rtw_ieee80211.c — WAPI/WPS/sec-IE getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-29
epic: E05
blocked_by: [W3-28]
estimate_loc: 200
---

## Goal

Port WAPI/WPS/sec-IE getter helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_get_wapi_ie`, `rtw_get_sec_ie`, `rtw_is_wps_ie`

## Notes

- L2: host harness with WAPI/WPS IE fixture bytes.

## Acceptance

- L0 build + L2 host unit tests for WAPI/WPS/sec-IE getters
