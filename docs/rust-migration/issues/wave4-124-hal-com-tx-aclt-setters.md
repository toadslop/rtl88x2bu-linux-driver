---
title: "[W4-124] Translate hal_com_rest.c — TX AC lifetime setters"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-124
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Under `CONFIG_TX_AC_LIFETIME`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_tx_aclt_set.rs`:

- `rtw_hal_set_tx_aclt_force_val`
- `rtw_hal_set_tx_aclt_conf`
- `rtw_hal_update_tx_aclt`

## Notes

- **Parallel lane:** complements dump formatters [W4-39](wave4-39-hal-com-tx-aclt-force-dump.md) / [W4-40](wave4-40-hal-com-tx-aclt-confs-dump.md) via new submodule (no edits to `hal_com_tx_aclt_confs.rs` dumps).
- **`rtw_hal_update_tx_aclt`** reads/writes `REG_LIFETIME_EN` and `REG_PKT_LIFE_TIME` via `rtw_read8`/`rtw_write8`/`rtw_write32` after folding `tx_aclt_confs[]`, role counts, and `tx_aclt_force_val` — it does **not** call `rtw_hal_set_hwreg` or walk macids.
- **`en == 0xFF`:** `rtw_hal_set_tx_aclt_force_val` stores `0xFF` (sentinel: `update` does not override); `rtw_hal_set_tx_aclt_conf` leaves `conf->en` unchanged. Do not conflate the two.
- Kconfig: `#if CONFIG_TX_AC_LIFETIME` (default 1 in `drv_conf.h`), same as W4-39 Rust cfg.

## Acceptance

- L0 + L1 for all three functions on pinned profile (`CONFIG_TX_AC_LIFETIME`)
- L2: setter force/conf `0xFF` semantics; `update` register read-modify-write values (lifetime enable bits, folded lifetimes) vs C — not `set_hwreg` stubs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none — dump slices on `master`)