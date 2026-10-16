---
title: "[W4-30] Translate hal_dm.c — boardType interface mapper"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-30
epic: E06
blocked_by: [W4-29]
estimate_loc: 200
---

## Goal

Port `boardType` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `CONFIG_USB_HCI` `INTERFACE_SELECT_USB` cases for 88x2bu (ext LNA/PA, minicard, BT combo, default)
- `CONFIG_PCI_HCI` branch preserved for build-all targets (same tables as C)
- `#[no_mangle]` export used by `Init_ODM_ComInfo` / remaining C in `hal_dm_rest.c`

## Notes

- Same-file follow-up to [W4-29](wave4-29-hal-dm-scaffold.md).
- Pure enum/bitfield mapping — no `phydm` struct walks.

## Acceptance

- L0 + L1 for `boardType`
- L2: host oracle for each `INTERFACE_SELECT_USB` case on USB HCI builds
