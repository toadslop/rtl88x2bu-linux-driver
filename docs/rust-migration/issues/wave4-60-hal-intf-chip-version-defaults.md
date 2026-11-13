---
title: "[W4-60] Translate hal_intf.c — chip version + def_value_init"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-60
epic: E06
blocked_by: [W4-59]
estimate_loc: 200
---

## Goal

Port chip identity and default HAL hooks from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c):

- `rtw_hal_read_chip_version`
- `rtw_hal_def_value_init` (registry / `hal_spec` defaults wired into `HAL_DATA`)

## Notes

- Depends on [W4-59](wave4-59-hal-intf-chip-configure.md) so `read_chip_info` ran before version
  fields are interpreted.
- Match `#[cfg]` gates on hardware types (`IS_HARDWARE_TYPE_*`) exactly — derive cfgs from C,
  do not hand-type chip constants.

## Acceptance

- L0 + L1 on this slice
- L3: module load unchanged on pinned kernel QEMU recipe
