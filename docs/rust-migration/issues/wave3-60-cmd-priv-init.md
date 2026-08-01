---
title: "[W3-60] Translate rtw_cmd.c — cmd/evt priv init and teardown"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-60
epic: E05
blocked_by: [W3-59]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rtw_cmd_rest.rs`](../../../rust/rtw_cmd_rest.rs):

- `_rtw_init_cmd_priv`
- `_rtw_init_evt_priv`
- `_rtw_free_cmd_priv`
- `_rtw_free_evt_priv`
- `rtw_init_cmd_priv`
- `rtw_init_evt_priv`
- `rtw_free_cmd_priv`
- `rtw_free_evt_priv`

## Notes

- Alloc/sema/queue init only — distinct from W3-50 IOL append encoders.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for cmd/evt priv init/teardown
