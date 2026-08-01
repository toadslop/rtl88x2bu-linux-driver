---
title: "[W3-79] stainfo free and sta priv lifecycle"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-79
epic: E05
blocked_by: [W3-78]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rust/rtw_sta_mgt_rest.rs`](../../../rust/rust/rtw_sta_mgt_rest.rs):

- `rtw_free_stainfo`
- `_rtw_init_sta_priv`
- `_rtw_free_sta_priv`
- `rtw_init_bcmc_stainfo`
- `rtw_mfree_stainfo`

## Notes

- Free path calls C-only teardown hooks; document shim boundaries in harness.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for stainfo free and sta priv lifecycle
