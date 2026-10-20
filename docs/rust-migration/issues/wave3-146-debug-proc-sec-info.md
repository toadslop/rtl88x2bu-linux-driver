---
title: "[W3-146] Translate rtw_debug_rest.c — proc_get_sec_info"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-146
epic: E05
blocked_by: [W3-145]
estimate_loc: 200
---

## Goal

Port `proc_get_sec_info` from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- Auth/encrypt algorithm lines
- `hw_decrypted`
- `DBG_SW_SEC_CNT` WEP/TKIP/AES/GCMP software encrypt/decrypt counters

## Notes

- Serial follow-up to [W3-145](wave3-145-debug-proc-mlme-leaf.md) on shared `rust/rtw_debug_proc.rs` and
  the same `CONFIG_RUST_RTW_DEBUG` / `rtw_debug_rest.c` guard lane — do not parallelize with W3-147.
- Reuse `rtw_debug` FFI helpers for `RTW_PRINT_SEL` where already present.

## Acceptance

- L0 with `CONFIG_PROC_DEBUG=y`
- L1 symbol parity on `proc_get_sec_info` in the swapped object (not `EXPORT_SYMBOL`)
- Output format byte-identical to C for a fixture `security_priv`
