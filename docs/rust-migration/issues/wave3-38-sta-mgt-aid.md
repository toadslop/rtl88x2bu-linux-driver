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

- Sta hash table and `rtw_free_all_stainfo` stay in C.
- L2: host harness with AID bitmap and pre-link sta ctl vectors.

## Acceptance

- L0 build + L2 host unit tests for AID and pre-link sta helpers
