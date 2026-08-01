---
title: "[W3-84] VHT IE build and assoc handlers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-84
epic: E05
blocked_by: [W3-83]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_vht.c`](../../../core/rtw_vht.c) to [`rust/rtw_vht.rs`](../../../rust/rtw_vht.rs):

- `rtw_build_vht_cap_ie`
- `rtw_build_vht_operation_ie`
- `rtw_vht_ies_attach`
- `rtw_vht_use_default_setting`
- `VHT_caps_handler`

## Notes

- W3-35/36/45 covered MCS/rate/restructure; this slice is build + handler path and supersedes the W3-36/W3-45 deferral of `rtw_vht_ies_attach` and assoc handlers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for VHT IE build and assoc handlers
