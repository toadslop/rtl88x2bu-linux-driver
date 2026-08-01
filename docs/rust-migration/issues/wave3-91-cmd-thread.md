---
title: "[W3-91] cmd thread loop"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-91
epic: E05
blocked_by: [W3-90]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rust/rtw_cmd_rest.rs`](../../../rust/rust/rtw_cmd_rest.rs):

- `rtw_cmd_thread`
- `rtw_stop_cmd_thread`
- `rtw_cmd_clr_isr`

## Notes

- Thread/semaphore coupled; W3-60/61 covered priv init and queue filter only.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for cmd thread loop
