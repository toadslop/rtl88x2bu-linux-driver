---
title: "[W3-38] Translate rtw_sta_mgt.c — AID and pre-link sta helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-38
epic: E05
blocked_by: [W3-37]
estimate_loc: 200
---

## Goal

Port AID allocation and pre-link sta helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `rtw_aid_alloc`
- `rtw_is_pre_link_sta`, `rtw_pre_link_sta_del`
- `rtw_pre_link_sta_ctl_init`, `rtw_pre_link_sta_ctl_deinit`, `rtw_pre_link_sta_ctl_reset`

## Notes

- Several targets are adapter/lock coupled: `rtw_pre_link_sta_del` and
  `rtw_pre_link_sta_ctl_reset` take `pre_link_sta_ctl->lock`, mutate sta lists,
  and call `rtw_free_stainfo()`; `rtw_aid_alloc` mutates `stapriv->sta_aid[]`
  under AP mode. Not pure leaf helpers like W3-19/W3-33.
- Sta hash table and `rtw_free_all_stainfo` stay in C; expect thin C shims or
  populated `sta_priv` / `pre_link_sta_ctl` fixtures for L2 (similar to W3-34).
- L2: host harness with AID bitmap and pre-link sta ctl vectors.

## Acceptance

- L0 build + L2 host unit tests for AID and pre-link sta helpers (lock ordering,
  adapter lifetime, and C/Rust boundary documented in harness)
