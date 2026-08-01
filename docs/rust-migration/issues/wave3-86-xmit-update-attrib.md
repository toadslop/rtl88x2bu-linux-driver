---
title: "[W3-86] update_attrib builders"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-86
epic: E05
blocked_by: [W3-85]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_xmit.c`](../../../core/rtw_xmit.c) to [`rust/rust/rtw_xmit.rs`](../../../rust/rust/rtw_xmit.rs):

- `update_attrib_phy_info`
- `update_attrib_vcs_info`
- `update_attrib_sec_info`
- `update_attrib`

## Notes

- First ~200 LOC of update_attrib cluster; W3-40/48/49 covered rate/QoS slices.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for update_attrib builders
