---
title: "[W3-169] Translate rtw_debug_rest.c — RF/BB reg dump (ex)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-169
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port remaining register dump formatters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_reg_dump.rs`](../../../rust/rtw_debug_reg_dump.rs) (extend W3-143 object):

- `bb_reg_dump_ex`
- `rf_reg_dump`

## Notes

- Follows closed [W3-143](wave3-143-debug-mac-bb-reg-dump.md) (`mac_reg_dump` / `bb_reg_dump`).
- `rf_reg_dump` uses `rtw_hal_read_rfreg` FFI (HAL read shim — same pattern as other debug dumps).
- Procfs entry points stay in C; Rust owns formatter bodies behind `CONFIG_RUST_RTW_DEBUG` guards.
- Pin chip ranges on 8822BU (`CONFIG_RTL8822B`).

## Acceptance

- L0 build + L1 symbol check on swapped symbols
- Optional L2: stub `rtw_hal_read_rfreg` / `rtw_read32` golden lines for one RF path window
