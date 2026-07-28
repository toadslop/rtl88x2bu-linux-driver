---
title: "[W3-37] Translate rtw_sta_mgt.c — match rule and access control"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-37
epic: E05
blocked_by: [W3-36]
estimate_loc: 200
---

## Goal

Port station match and access-control helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `test_st_match_rule`
- `_rtw_access_ctrl`, `rtw_access_ctrl`

## Notes

- Sta init/free/hash paths stay in C (adapter locks, list management).
- L2: host harness under `tests/host/sta_mgt/` with match-rule and ACL vectors.

## Acceptance

- L0 build + L2 host unit tests for match rule and access control
