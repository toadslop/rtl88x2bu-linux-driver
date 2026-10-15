---
title: "[W4-24] Translate hal_halmac.c — FW mem, LED mode, phy param val mappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-24
epic: E06
blocked_by: [W4-23]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with additional HALMAC enum/bitfield
mappers from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_fw_mem_drv2halmac`
- `_led_mode_drv2halmac`
- `_phy_parameter_val_drv2halmac`

## Notes

- Same-file follow-up in the hal_halmac lane after [W4-23](wave4-23-hal-halmac-usb-fifo-mappers.md).
- `_phy_parameter_val_drv2halmac` is pure bitmask math — no `struct rtw_phy_parameter` walk.
- Full `_phy_parameter_drv2halmac` struct translation stays in C for a later slice.

## Acceptance

- L0 + L1 for the three mappers
- L2: FW mem cases (`FW_EMEM` + `tx_stop`), LED mode table, and phy param val with/without mask enable
