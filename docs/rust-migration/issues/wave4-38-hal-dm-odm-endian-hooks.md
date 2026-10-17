---
title: "[W4-38] Translate hal_dm.c — Init_ODM big-endian hook block"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-38
epic: E06
blocked_by: [W4-30]
estimate_loc: 180
---

## Goal

Port the `convert_to_big_endian` helper and the four `odm_cmn_info_hook` calls that use it inside `Init_ODM_ComInfo` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `ODM_CMNINFO_BAND`, `ODM_CMNINFO_SEC_MODE`, `ODM_CMNINFO_BW`, `ODM_CMNINFO_NET_CLOSED` hooks
- Export `rtw_rust_hal_dm_init_odm_endian_hooks(...)` invoked from remaining C `Init_ODM_ComInfo`

## Notes

- Same-file follow-up to [W4-30](wave4-30-hal-dm-board-type.md); does not port full `Init_ODM_ComInfo`.
- L2 compares pointer/endian layout against C oracle for synthetic field values.

## Acceptance

- L0 + L1 for the Rust endian-hook helper
- L2: fixture HalData/security/band/net_closed → expected hook addresses/values
