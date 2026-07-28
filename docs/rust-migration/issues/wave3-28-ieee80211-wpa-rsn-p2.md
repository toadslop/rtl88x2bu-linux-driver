---
title: "[W3-28] Translate rtw_ieee80211.c — WPA/RSN IE parse"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-28
epic: E05
blocked_by: [W3-27]
estimate_loc: 200
---

## Goal

Port WPA/RSN IE parsing from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_parse_wpa_ie`, `rtw_rsne_info_parse`, `rtw_parse_wpa2_ie`

## Notes

- Builds on W3-27 cipher suite getters.
- L2: freeze WPA/RSN IE vectors from C oracle before port.

## Acceptance

- L0 build + L2 host unit tests for WPA/RSN IE parse
