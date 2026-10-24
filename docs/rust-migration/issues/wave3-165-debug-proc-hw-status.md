---
title: "[W3-165] Translate rtw_debug_rest.c — proc hw_status check"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-165
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port hardware status proc hooks from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_hw.rs`](../../../rust/rtw_debug_proc_hw.rs):

- `proc_get_hw_status`
- `proc_set_hw_status`

## Notes

- Getter branches on `regsty->check_hw_status` and RX FIFO overflow triple-flag state.
- Setter updates `registry_priv.check_hw_status` (0/1) via `dvobj_to_regsty`.

## Acceptance

- L0 + L1 on both symbols
- All three getter branches and setter clamping match C
