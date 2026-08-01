---
title: "[W3-61] Translate rtw_cmd.c — cmd/evt queue enqueue and filter"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-61
epic: E05
blocked_by: [W3-60]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rtw_cmd_rest.rs`](../../../rust/rtw_cmd_rest.rs):

- `_rtw_enqueue_cmd`
- `_rtw_dequeue_cmd`
- `rtw_cmd_filter`
- `rtw_enqueue_cmd`
- `rtw_dequeue_cmd`
- `rtw_free_cmd_obj`
- `rtw_enqueue_evt`
- `rtw_free_evt_obj`
- `rtw_evt_notify_isr`

## Notes

- Spinlock queues + filter gates. Cmd thread and per-cmd handlers stay in C.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for cmd/evt queue enqueue/filter
