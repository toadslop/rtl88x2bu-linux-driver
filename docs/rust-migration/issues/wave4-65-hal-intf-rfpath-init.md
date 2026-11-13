---
title: "[W4-65] Translate hal_intf.c — hal_rfpath_init"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-65
epic: E06
blocked_by: [W4-64]
estimate_loc: 200
---

## Goal

Port `rtw_hal_rfpath_init` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs).

## Notes

- Reuse Rust `_dump_rf_path` from W4-08 for debug dumps; keep `CONFIG_RTL8814A` branch behind the same `#[cfg]` gates as C.
- Registry/eeprom trx_path_bmp intersection and `rtw_restrict_trx_path_bmp_by_trx_num_lmt` stay on the C side via `rtw_rust_hal_intf_rf_path_fields` / dedicated shims until a later slice needs them in Rust.
- Return `_FAIL` / `_SUCCESS` paths must match C log lines and early exits.

## Acceptance

- L0 + L1 for `rtw_hal_rfpath_init`
- L2: host fixture adapters covering registry override, eeprom override, and restrict-trx failure paths (where harnessable without MMIO)
