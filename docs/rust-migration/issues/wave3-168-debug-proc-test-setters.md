---
title: "[W3-168] Translate rtw_debug_rest.c — proc manufacturing test setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-168
epic: E05
blocked_by: [W3-132]
estimate_loc: 200
---

## Goal

Port proc **setter** hooks that forward into Rust test-hook globals from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_test.rs`](../../../rust/rtw_debug_proc_test.rs):

- `proc_set_fwdl_test_case`
- `proc_set_del_rx_ampdu_test_case`
- `proc_set_wait_hiq_empty`
- `proc_set_sta_linking_test`
- `proc_set_ap_linking_test` (`CONFIG_AP_MODE`)

## Notes

- Rust hook storage lives in [`rust/rtw_debug_test_hooks.rs`](../../../rust/rtw_debug_test_hooks.rs) from [W3-132](wave3-132-debug-test-hooks.md); this slice owns proc `copy_from_user` / `sscanf` parsing only.
- Keep `CONFIG_RUST_RTW_DEBUG` static mirrors in C when non-Rust builds need them.

## Acceptance

- L0 + L1 on all symbols present in the pinned kernel config
- AP-only setter excluded from strict L1 when `CONFIG_AP_MODE` is disabled (STA profile)
