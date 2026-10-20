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

- Same-file follow-up to [W3-145](wave3-145-debug-proc-mlme-leaf.md) (shared `rtw_debug_proc` module).
- Reuse `rtw_debug` FFI helpers for `RTW_PRINT_SEL` where already present.

## Acceptance

- L0 + L1 symbol parity
- Output format byte-identical to C for a fixture `security_priv`
