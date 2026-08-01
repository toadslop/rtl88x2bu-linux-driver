---
title: "[W3-84] VHT IE build and assoc handlers"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-84
epic: E05
blocked_by: [W3-83]
estimate_loc: 430
---

## Goal

Port helpers from [`core/rtw_vht.c`](../../../core/rtw_vht.c) to [`rust/rtw_vht.rs`](../../../rust/rtw_vht.rs):

- `rtw_build_vht_cap_ie` (~140 LOC)
- `rtw_build_vht_operation_ie` (~36 LOC)
- `rtw_vht_ies_attach` (~33 LOC)
- `rtw_vht_use_default_setting` (~111 LOC)
- `VHT_caps_handler` (~111 LOC)

## Notes

- **Multi-PR slice (~430 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each).
- W3-35/36/45 covered MCS/rate/restructure; this slice is build + handler path and supersedes the W3-36/W3-45 deferral of `rtw_vht_ies_attach` and assoc handlers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for VHT IE build and assoc handlers
