---
title: "[W3-78] stainfo alloc"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-78
epic: E05
blocked_by: [W3-77]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `rtw_alloc_stainfo`

## Notes

- Alloc path mutates sta hash under lock; expect adapter/sta_priv fixtures for L2.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for stainfo alloc
