---
title: "[W3-27] Translate rtw_ieee80211.c — WPA/RSN cipher suite getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-27
epic: E05
blocked_by: [W3-26]
estimate_loc: 200
---

## Goal

Port WPA/RSN cipher suite getter helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_get_wpa_cipher_suite`, `rtw_get_rsn_cipher_suite`
- `rtw_get_akm_suite_bitmap`

## Notes

- Pure string/byte parsing; no adapter state.
- L2: host harness with known cipher suite byte sequences.

## Acceptance

- L0 build + L2 host unit tests for cipher suite getters
