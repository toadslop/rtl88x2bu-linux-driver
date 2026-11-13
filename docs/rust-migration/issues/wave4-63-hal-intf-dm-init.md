---
title: "[W4-63] Translate hal_intf.c — dm_init + dm_deinit"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-63
epic: E06
blocked_by: [W4-62]
estimate_loc: 80
---

## Goal

Port HAL DM lifecycle hooks from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_dm_init`
- `rtw_hal_dm_deinit`

## Notes

- Primary-adapter guard only; delegate `dm_init` / `dm_deinit` through existing `hal_func` shims.
- Preserve `IQKSpinLock` init/free order and `CONFIG_TXPWR_PG_WITH_PWR_IDX` / `phy_load_tx_power_ext_info` calls via thin C FFI (same pattern as W4-61/62 data paths).
- Extend existing L1 `hal_intf` oracle with reference stubs for both symbols.

## Acceptance

- L0 build links `rust/hal_intf.o` + `hal_intf_rest.o`
- L1: `make rust-check-symbols-hal-intf` includes `rtw_hal_dm_init` / `rtw_hal_dm_deinit`
