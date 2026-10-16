---
title: "[W4-30] Translate hal_dm.c — Init_ODM_ComInfo board-type block"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-30
epic: E06
blocked_by: [W4-29]
estimate_loc: 200
---

## Goal

Port the **board-type block** inside `Init_ODM_ComInfo` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) (lines building `ODM_CMNINFO_BOARD_TYPE` from `PHAL_DATA_TYPE` efuse / PA / LNA / BT fields and the matching `odm_cmn_info_init` side effects):

- Map `ExternalLNA_2G`, `external_lna_5g`, `ExternalPA_2G`, `external_pa_5g`, and `EEPROMBluetoothCoexist` into `odm_board_type` flags (`ODM_BOARD_EXT_*`, `ODM_BOARD_BT`) exactly as C
- Call `odm_cmn_info_init` for `ODM_CMNINFO_EXT_LNA`, `ODM_CMNINFO_5G_EXT_LNA`, `ODM_CMNINFO_EXT_PA`, `ODM_CMNINFO_5G_EXT_PA` when the corresponding HalData fields are non-zero
- Finish with `odm_cmn_info_init(..., ODM_CMNINFO_BOARD_TYPE, odm_board_type)`
- Export a `#[no_mangle]` helper invoked from remaining `Init_ODM_ComInfo` in `hal_dm_rest.c`

## Notes

- Same-file follow-up to [W4-29](wave4-29-hal-dm-scaffold.md).
- **`boardType(InterfaceSel)` is dead code** (no in-tree callers). Do **not** port it; L2 targets the live HalData→ODM path above.
- No watchdog, MMIO, or full `Init_ODM_ComInfo` port beyond this block.

## Acceptance

- L0 + L1 for the Rust board-type helper used from `Init_ODM_ComInfo`
- L2: host oracle with fixture `HAL_DATA_TYPE` fields → expected `ODM_CMNINFO_BOARD_TYPE` value and ext LNA/PA/BT `odm_cmn_info_init` side effects
