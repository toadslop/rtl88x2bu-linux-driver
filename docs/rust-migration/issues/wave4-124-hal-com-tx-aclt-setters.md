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
- `rtw_hal_set_hwreg` / macid walk in `rtw_hal_update_tx_aclt` uses existing Rust shims where already ported.

## Acceptance

- L0 + L1 for all three functions on pinned profile
- L2: host fixture checks scaled vo_vi/be_bk fields and LT register write args vs C

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none — dump slices on `master`)