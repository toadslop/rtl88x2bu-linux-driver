---
title: "[W3-42] Translate rtw_ieee80211.c — HT MCS bitmap and AMSDU mode"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-42
epic: E05
blocked_by: [W3-41]
estimate_loc: 200
---

## Goal

Port HT MCS and AMSDU mode helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_ht_mcsset_to_nss`, `rtw_ht_mcs_set_to_bitmap`
- `rtw_set_spp_amsdu_mode`, `rtw_check_amsdu_disable`

## Notes

- Builds on W3-32 frame/HT MCS work; these are pure bitmap/mode mutators on IE bytes.
- Debug `dump_ht_*` printers stay in C.
- L2: extend `tests/host/ie/` with MCS set and RSN AMSDU mode fixtures.

## Acceptance

- L0 build + L2 host unit tests for HT MCS bitmap and AMSDU mode helpers
