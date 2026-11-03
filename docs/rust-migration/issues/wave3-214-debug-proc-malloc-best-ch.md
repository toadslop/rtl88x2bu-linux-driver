---
title: "[W3-214] Translate rtw_debug_rest.c — proc malloc cnt + best channel leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-214
epic: E05
blocked_by: [W3-211]
estimate_loc: 160
---

## Goal

Port optional debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_misc.rs`](../../../rust/rtw_debug_proc_misc.rs):

- `proc_get_malloc_cnt` (`DBG_MEMORY_LEAK`)
- `proc_get_best_channel` / `proc_set_best_channel` (`CONFIG_FIND_BEST_CHANNEL`)

## Notes

- `proc_get_malloc_cnt` reads kernel `atomic_t` exports — expose via existing debug FFI or small shim in C.
- Best-channel procs scan `rf_ctl_t::channel_set` only (no HAL MMIO).
- `proc_get_mac_rptbuf` stays in C (`rtw_read32`/`rtw_write16` — not L2-safe on host).

## Acceptance

- L0 with relevant Kconfig symbols on pinned profile
- L1 on all handlers
- L2: malloc counter lines; best-channel proc_get/set with fixture `channel_set`
