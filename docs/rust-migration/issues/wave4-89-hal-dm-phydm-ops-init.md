---
title: "[W4-89] Translate hal_dm.c — phydm ops init + record_ra_info"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-89
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Port PHYDM vtable wiring from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_ops_func_init` — assign `record_ra_info` on `p_phydm->dm_ra_table` and optional `fill_desc_dyntxpwr`
- `record_ra_info` — copy RA mask / HT/VHT fields into `cmn_sta_info` via existing `phydm_ra_info_*` FFI

## Notes

- **Parallel lane:** disjoint from hal_intf and hal_com work.
- Called from the `Init_ODM` tail ([W4-88](wave4-88-hal-dm-init-odm-hook-tail.md)); keep call order compatible if both land in one stack.
- Does not port watchdog, turbo EDCA, or full `SetHalODMVar`.

## Acceptance

- L0 + L1 for exported symbols
- L2: host oracle with mock `dm_struct` / `cmn_sta_info` — RA mask bits land in the same fields as the C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)