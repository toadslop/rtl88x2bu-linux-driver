---
title: "[W4-23] Translate hal_halmac.c — USB mode + FIFO sel mappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-23
epic: E06
blocked_by: [W4-22]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with USB/HCI-related enum mappers from
[`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_usb_mode_drv2halmac` (`CONFIG_USB_HCI`)
- `_fifo_sel_drv2halmac`

## Notes

- Starts Wave 4 USB/HCI mapper coverage in the hal_halmac lane (used by
  `rtw_halmac_switch_usb_mode` and `rtw_halmac_dump_fifo`).
- No live USB device or `halmac_api` calls in this slice — translation tables only.
- **Dependency rationale:** same Rust module and Makefile object line as W4-22.

## Acceptance

- L0 + L1 for both mappers (guard `_usb_mode_drv2halmac` with `CONFIG_USB_HCI` like C)
- L2: vectors for `RTW_USB_SPEED_*` values and FIFO sel indices 0–5 plus default fallbacks
