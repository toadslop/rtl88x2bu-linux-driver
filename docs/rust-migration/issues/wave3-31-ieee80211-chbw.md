---
title: "[W3-31] Translate rtw_ieee80211.c — chbw grouping and sync"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-31
epic: E05
blocked_by: [W3-30]
estimate_loc: 200
---

## Goal

Port channel/bandwidth grouping helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_ies_get_chbw`, `rtw_bss_get_chbw`
- `rtw_is_chbw_grouped`, `rtw_sync_chbw`

## Notes

- Depends on W3-19/W3-20 RF channel helpers for chbw semantics.
- L2: host harness with grouped/sync chbw vectors.

## Acceptance

- L0 build + L2 host unit tests for chbw grouping/sync
