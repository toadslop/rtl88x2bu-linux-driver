---
title: "[W4-33] Translate hal_usb.c — sync vendor read8/16/32"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-33
epic: E06
blocked_by: [W4-28]
estimate_loc: 200
---

## Goal

Port synchronous USB vendor read helpers from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- `usb_read8`, `usb_read16`, `usb_read32` — request `0x05`, wvalue from addr low 16 bits, `usb_vendor_wvalue_fixup`, delegate to `usbctrl_vendorreq`
- Keep `usb_set_intf_ops` wiring unchanged; only move the three read symbols to Rust

## Notes

- hal_usb lane follow-up to [W4-28](wave4-28-hal-usb-vendor-wvalue.md); reuses existing `usb_vendor_wvalue_fixup`.
- C retains `usbctrl_vendorreq` / `usb_read_mem` / port paths until later slices.

## Acceptance

- L0 build with `CONFIG_RUST` + `CONFIG_USB_HCI`
- L1: `usb_read8` / `usb_read16` / `usb_read32` exported from `rust/hal_usb.o`
- L2: host harness mocks `usbctrl_vendorreq` and checks wvalue fixup + length per width
