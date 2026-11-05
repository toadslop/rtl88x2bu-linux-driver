---
title: "[W3-216] Translate rtw_debug_rest.c — proc RF4CE + silent reset leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-216
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port optional debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_sreset.rs`](../../../rust/rtw_debug_proc_sreset.rs):

- `proc_get_rf4ce_state` / `proc_set_rf4ce_state` (`CONFIG_RF4CE_COEXIST`, nested under
  `CONFIG_BT_COEXIST` in C)
- `proc_get_sreset` / `proc_set_sreset` (`DBG_CONFIG_ERROR_DETECT`)

## Notes

- RF4CE setters call `rtw_btcoex_SetRf4ceState` via FFI; getters use `rtw_btcoex_GetRf4ceLinkState`.
- Silent-reset procs touch `rtw_sreset_get_*` / test globals — expose via existing debug test FFI where possible.
- Parallel with [W3-215](wave3-215-debug-proc-btcoex-dbg.md) (no shared Rust file).

## Acceptance

- L0 with relevant Kconfig symbols on pinned profile
- L1 on all listed handlers
- L2: rf4ce connected/disconnected strings; sreset proc_set mode clamp
