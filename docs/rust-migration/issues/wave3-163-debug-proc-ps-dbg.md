---
title: "[W3-163] Translate rtw_debug_rest.c — proc ps debug counters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-163
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port power-save debug proc hooks from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ps.rs`](../../../rust/rtw_debug_proc_ps.rs):

- `proc_get_ps_dbg_info`
- `proc_set_ps_dbg_info`

## Notes

- Reads/writes `dvobj->drv_dbg` (`struct debug_priv`) counters — reuse bindgen layout from existing debug modules.
- Setter accepts `ps_dbg_cmd_id == 1` to zero the whole `debug_priv` struct.

## Acceptance

- L0 + L1 on both symbols
- All printed counter lines and clear command behavior match C
