---
title: "[W3-149] Translate rtw_debug_rest.c — proc defs + register peek/poke"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-149
epic: E05
blocked_by: [W3-148]
estimate_loc: 170
---

## Goal

Port proc debug helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) under `CONFIG_PROC_DEBUG`:

- `proc_get_defs_param` / `proc_set_defs_param`
- `proc_set_write_reg` — `rtw_write8/16/32` dispatch
- `proc_get_read_reg` / `proc_set_read_reg` — module-local read addr/len state + `rtw_read8/16/32`

## Notes

- Keep `proc_get_read_addr` / `proc_get_read_len` static state in the Rust object (or equivalent `static mut` with C ABI) so
  read_reg behavior matches C.
- Chains the debug proc lane after [W3-148](wave3-148-debug-proc-roam-setters.md).

## Acceptance

- L0 + L1 on all five symbols in `rtw_debug_proc.o`
- Register write length switch and read “address not initialized” string unchanged
