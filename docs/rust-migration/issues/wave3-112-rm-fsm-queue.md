---
title: "[W3-112] rm_fsm obj/queue/clock"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-112
epic: E05
blocked_by: [W3-111]
estimate_loc: 220
---

## Goal

Port helpers from [`core/rtw_rm_fsm.c`](../../../core/rtw_rm_fsm.c) to [`rust/rtw_rm_fsm.rs`](../../../rust/rtw_rm_fsm.rs):

- `rm_enqueue_ev`
- `rm_set_clock`
- `rm_alloc_clock`
- `rm_cancel_clock`
- `rm_free_clock`
- `rm_free_rmobj`
- `rm_alloc_rmobj`
- `rm_enqueue_rmobj`
- `is_list_linked`

## Notes

- RM FSM object/queue/clock lifecycle helpers; medium coupling via adapter RM state.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for rm_fsm obj/queue/clock helpers
