---
title: "[W3-68] Translate rtw_mlme_ext.c — mgnt frame attribute builders"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-68
epic: E05
blocked_by: [W3-67]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `update_monitor_frame_attrib`
- `update_mgntframe_attrib`
- `update_mgntframe_attrib_addr`
- `update_mgntframe_subtype`

## Notes

- Frame metadata only; no TX/HAL path.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mgnt frame attribute builders
