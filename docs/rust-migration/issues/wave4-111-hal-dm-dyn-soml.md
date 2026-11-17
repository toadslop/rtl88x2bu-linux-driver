---
title: "[W4-111] Translate hal_dm.c — dynamic SOML helpers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-111
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Under `CONFIG_DYNAMIC_SOML`, port from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_dyn_soml_byte_update`
- `rtw_dyn_soml_para_set`
- `rtw_dyn_soml_config`

## Notes

- **Parallel lane:** registry-driven `dyn_soml_en` branches only; `HW_VAR_SET_SOML_PARAM` stays C `set_hwreg` FFI.
- Delegate to `phydm_soml_bytes_acq` / `phydm_adaptive_soml_para_set`.

## Acceptance

- L0 + L1 when `CONFIG_DYNAMIC_SOML` is enabled
- L2: mock registry + PHYDM record para-set calls for `dyn_soml_en` 0/1/2 paths

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)