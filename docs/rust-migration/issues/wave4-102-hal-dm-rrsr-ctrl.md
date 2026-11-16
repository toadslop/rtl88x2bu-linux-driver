---
title: "[W4-102] Translate hal_dm.c — RRSR set and dynamic enable"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-102
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Port rate-mask register helpers from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_set_rrsr` — registry override vs argument, `odm_cmn_info_update` + optional `phydm_rrsr_set_register`
- `rtw_phydm_dyn_rrsr_en` — `phydm_rrsr_en` delegate

## Notes

- **Parallel lane:** leaf PHYDM FFI wrappers; no overlap with ODM var or IQK slices.
- Preserve `registrypriv.set_rrsr_value == 0xFFFFFFFF` sentinel behavior.

## Acceptance

- L0 + L1 for both exports
- L2: mock `dm_struct` + registry fixture records RRSR value passed to PHYDM stubs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)