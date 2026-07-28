---
title: "[W3-30] Translate rtw_ieee80211.c — string and MAC address helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-30
epic: E05
blocked_by: [W3-29]
estimate_loc: 200
---

## Goal

Port string/MAC address conversion helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `str_2char2num`, `key_2char2num`, `macstr2num`, `convert_ip_addr`
- `rtw_check_invalid_mac_address`, `rtw_macaddr_cfg`

## Notes

- Reuse `MacAddr` domain type from A1 where applicable.
- `rtw_get_mac_addr_intel` stays in C (platform I/O).
- L2: host harness for MAC/IP string conversion vectors.

## Acceptance

- L0 build + L2 host unit tests for string/MAC helpers
