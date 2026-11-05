---
title: "[W3-215] Translate rtw_debug_rest.c — proc btcoex debug leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-215
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port Bluetooth coexistence debug proc handlers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_btcoex.rs`](../../../rust/rtw_debug_proc_btcoex.rs) (`CONFIG_BT_COEXIST`):

- `proc_get_btcoex_dbg` / `proc_set_btcoex_dbg`
- `proc_get_btcoex_info`

## Notes

- Setters/getters call `rtw_btcoex_GetDBG` / `rtw_btcoex_SetDBG` / `rtw_btcoex_DisplayBtCoexInfo` via thin C FFI.
- Parallel **debug proc** lane (disjoint Rust module from misc/phy/wow lanes).
- `proc_set_btcoex_dbg` accepts one or two `%x` module masks (same sscanf rules as C).

## Acceptance

- L0 with `CONFIG_BT_COEXIST`
- L1 on all three handlers
- L2: proc_get/set dbg mask vectors; btcoex_info header line from fixture adapter
