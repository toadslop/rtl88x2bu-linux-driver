---
title: "[W3-43] Translate rtw_ieee80211.c — P2P IE merge and delete"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-43
epic: E05
blocked_by: [W3-42]
estimate_loc: 200
---

## Goal

Port P2P IE merge/delete helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_get_p2p_merged_ies_len`, `rtw_p2p_merge_ies`, `rtw_set_p2p_attr_content`
- `rtw_del_p2p_ie`, `rtw_del_p2p_attr`, `rtw_bss_ex_del_p2p_ie`, `rtw_bss_ex_del_p2p_attr`

## Notes

- In-place buffer mutation on IE blobs; same pattern as W3-03 IE parse.
- P2P action frame handlers stay in C.
- L2: extend `tests/host/ie/` with before/after merge/delete byte vectors.

## Acceptance

- L0 build + L2 host unit tests for P2P IE merge and delete helpers
