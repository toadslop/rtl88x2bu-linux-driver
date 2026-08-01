---
title: "[W3-88] join_cmd_hdl"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-88
epic: E05
blocked_by: [W3-87]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rust/rtw_mlme_ext_rest.rs`](../../../rust/rust/rtw_mlme_ext_rest.rs):

- `join_cmd_hdl`

## Notes

- HAL-coupled cmd handler (~230 LOC); adapter fixtures required for L2.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for join_cmd_hdl
