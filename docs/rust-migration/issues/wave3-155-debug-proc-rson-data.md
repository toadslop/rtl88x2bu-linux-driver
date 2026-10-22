---
title: "[W3-155] Translate rtw_debug_rest.c — proc rson_data get/set"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-155
epic: E05
blocked_by: [W3-154]
estimate_loc: 80
---

## Goal

Port `CONFIG_RTW_REPEATER_SON` proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_get_rson_data` — `rtw_rson_get_property_str` formatter
- `proc_set_rson_data` — `sscanf` field/value → `rtw_rson_set_property`

## Notes

- Thin FFI to existing C `rtw_rson_*` helpers is acceptable for this slice.
- Chains debug proc lane after [W3-154](wave3-154-debug-proc-war-offload-mdns-p1.md).

## Acceptance

- L0 with `CONFIG_RTW_REPEATER_SON=y` on pinned profile
- L1 on both proc symbols
- Invalid field/value paths log like C without crashing
