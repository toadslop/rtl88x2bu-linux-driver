---
title: "[W4-85] Translate hal_dm.c — Init_ODM adaptivity + traffic hooks"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-85
epic: E06
blocked_by: [W4-84]
estimate_loc: 200
---

## Goal

Port mid `Init_ODM_ComInfo` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- Type GPA/LNA, rfe, crystal, kfree / antenna / efuse `odm_cmn_info_init` block (after board-type Rust helper)
- Adaptivity `odm_cmn_info_hook` + `phydm_adaptivity_info_init` trio
- `halrf_cmn_info_init` thermal / pwr tracking block
- Optional `rtw_odm_adaptivity_config_msg`, `CONFIG_IQK_PA_OFF`, and calls to `rtw_hal_update_*_fw_offload_cap` (Rust exports from W4-79)
- Pointer hooks: antenna div, mp_mode, bb op mode, tx/rx uni, forced rate, sec channel offset

## Notes

- Same-file follow-up to [W4-84](wave4-84-hal-dm-phydm-priv-odm-iface.md).
- Endian/narrowband hook block remains W4-38 — call existing Rust helper from C shim here.

## Acceptance

- L0 + L1 for the new Init_ODM helper segment
- L2 spot oracle for adaptivity + halrf_cmn_info_init numeric fields

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-84