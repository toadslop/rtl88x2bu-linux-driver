---
title: "[W3-220] Translate rtw_debug_rest.c — proc monitor + xmit block leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-220
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port monitor / debug xmit procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_monitor.rs`](../../../rust/rtw_debug_proc_monitor.rs):

- `proc_get_monitor` / `proc_set_monitor`
- `proc_get_simple_config` / `proc_set_simple_config` (`RTW_SIMPLE_CONFIG`)
- `proc_get_xmit_block` / `proc_set_xmit_block` (`DBG_XMIT_BLOCK`)

## Notes

- `proc_set_monitor` channel path calls `set_channel_bwmode` via FFI (channel set stays in C).
- `proc_get_xmit_block` uses `dump_xmit_block` — export formatter through C for L2 golden strings.
- Parallel debug lane root.

## Acceptance

- L0 with monitor + optional Kconfig fragments enabled on pinned profile
- L1 on all listed handlers
- L2: monitor disable banner; simple_config enable line; xmit_block mode set/get
