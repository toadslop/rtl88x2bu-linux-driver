---
title: "[W3-36] Translate rtw_vht.c — VHT IE restructure"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-36
epic: E05
blocked_by: [W3-35]
estimate_loc: 200
---

## Goal

Port VHT IE restructure helper from [`core/rtw_vht.c`](../../../core/rtw_vht.c) to [`rust/rtw_vht.rs`](../../../rust/rtw_vht.rs):

- `rtw_restructure_vht_ie`

## Notes

- `rtw_restructure_vht_ie` is adapter/HAL/rfctl coupled in C (not a pure IE transform);
  expect thin C shims or populated adapter/rfctl fixtures for L2, similar to W3-34/37.
- Builds on W3-35 MCS map helpers.
- `rtw_vht_ies_attach`/`detach` and assoc handlers stay in C.
- L2: host harness with input/output IE byte vectors.

## Acceptance

- L0 build + L2 host unit tests for VHT IE restructure (adapter/shim requirements
  documented in harness)
