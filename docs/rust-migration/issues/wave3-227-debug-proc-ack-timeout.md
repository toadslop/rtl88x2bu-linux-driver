---
title: "[W3-227] Translate rtw_debug_rest.c — proc ACK timeout leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-227
epic: E05
blocked_by: []
estimate_loc: 160
---

## Goal

Port ACK timeout debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ack.rs`](../../../rust/rtw_debug_proc_ack.rs):

- `proc_get_ack_timeout` / `proc_set_ack_timeout`

## Notes

- Getter reads `REG_ACKTO` (+ `REG_ACKTO_CCK_8821C` when `CONFIG_RTL8821C`) via existing `rtw_read8` FFI.
- Setter writes the same registers through `rtw_write8` FFI — no new HAL surface.
- L2 host oracle stubs register read/write hooks and asserts sscanf + byte values written.

## Acceptance

- L0 `CONFIG_PROC_DEBUG`
- L1 on both handlers
- L2: single-arg and dual-arg (`8821C`) timeout vectors; invalid parameter count paths
