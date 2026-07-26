---
title: "[W3-03] Translate rtw_ieee80211 IE parse helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-03
epic: E05
blocked_by: [W3-01]
estimate_loc: 200
---

## Goal

Port `rtw_get_ie`, `rtw_get_ie_ex`, `rtw_ies_remove_ie` from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211.rs`](../../../rust/rtw_ieee80211.rs).

## Acceptance

- L0 + L2 (`tests/host/ie/`) green
