---
title: "[W4-73] Translate hal_intf.c — hwreg/def/odm dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-73
epic: E06
blocked_by: [W4-72]
estimate_loc: 80
---

## Goal

Port thin `hal_func` dispatch wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_set_hwreg` / `rtw_hal_get_hwreg`
- `rtw_hal_set_def_var` / `rtw_hal_get_def_var`
- `rtw_hal_set_odm_var` / `rtw_hal_get_odm_var`
- `rtw_hal_notch_filter`

## Notes

- Handler pointer dispatch only — no MMIO in this slice.
- **Dependency rationale:** continues hal_intf lane after inirp ([W4-72](wave4-72-hal-intf-inirp.md)); Makefile object swap stays on `rust/hal_intf.o`.

## Acceptance

- L0 + L1 symbol parity for all exported symbols
- L3 module load smoke on pinned QEMU recipe

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-72