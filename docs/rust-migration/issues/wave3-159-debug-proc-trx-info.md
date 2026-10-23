---
title: "[W3-159] Translate rtw_debug_rest.c — proc trx_info get/reset"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-159
epic: E05
blocked_by: [W3-158]
estimate_loc: 210
---

## Goal

Port trx debug proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_trx.rs`](../../../rust/rtw_debug_proc_trx.rs):

- `proc_reset_trx_info` — zero recv dbg counters on `0` command
- `proc_get_trx_info` — WMM EDCA, queue dumps, HAL MAC TXFIFO dump hook

## Notes

- `dump_os_queue`, `dump_rx_bh_tk`, and `read_wmmedca_reg` remain C FFI (HAL/xmit glue).
- Split only if `git diff --shortstat` exceeds 250 lines during implementation.
- Chains after [W3-158](wave3-158-debug-proc-ap-info.md).

## Acceptance

- L0 with `CONFIG_PROC_DEBUG` on pinned profile
- L1 on getter and setter symbols
- Counter reset and WMM banner lines match C output
