---
title: "[W4-84] Translate hal_dm.c — phydm_priv_init + Init_ODM iface block"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-84
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port early PHYDM setup from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_priv_init` (`ODM_CMNINFO_PLATFORM`, `phydm->adapter` assignment)
- Start of `Init_ODM_ComInfo` through regulation/DFS hooks (before board-type block already in Rust):
  - `phydm_op_mode`, `rtw_odm_init_ic_type`, interface type (`RTW_GSPI` vs `rtw_get_intf_type`)
  - `ODM_CMNINFO_MP_TEST_CHIP`, patch ID, wifi test, optional `CONFIG_ADVANCE_OTA`
  - `ODM_CMNINFO_RF_TYPE`, `rtw_hal_set_odm_var(HAL_ODM_REGULATION)`, optional `CONFIG_DFS_MASTER` hook

## Notes

- **Parallel lane:** `rust/hal_dm.rs` only — disjoint from hal_intf tranche 17.
- Does not re-port W4-30 board-type or W4-38 endian slices.
- Export helpers called from remaining `Init_ODM_ComInfo` in `hal_dm_rest.c`.

## Acceptance

- L0 + L1 for new `#[no_mangle]` entry points
- L2 host oracle comparing interface/IC type `odm_cmn_info_init` calls vs C oracle (fixture HalData + registry)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)