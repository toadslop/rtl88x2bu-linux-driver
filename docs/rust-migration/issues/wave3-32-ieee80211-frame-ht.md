---
title: "[W3-32] Translate rtw_ieee80211.c — frame header and HT MCS helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-32
epic: E05
blocked_by: [W3-31]
estimate_loc: 200
---

## Goal

Port frame header and HT MCS helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `ieee80211_is_empty_essid`, `ieee80211_get_hdrlen`, `rtw_action_frame_parse`
- `rtw_ht_mcs_rate`, `rtw_ht_cap_get_rx_nss`, `rtw_ht_cap_get_tx_nss`

## Notes

- L2: host harness with frame header and HT MCS oracle vectors.

## Acceptance

- L0 build + L2 host unit tests for frame/HT helpers
