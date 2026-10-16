---
title: "[W4-28] Translate hal_usb.c — vendor wvalue WLANON page0 fixup"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-28
epic: E06
blocked_by: [W4-25]
estimate_loc: 200
---

## Goal

Deduplicate the repeated `CONFIG_RTL8710B` wvalue adjustment in
[`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) by moving it to
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- `usb_vendor_wvalue_fixup(u16 wvalue) -> u16` (or equivalent `#[no_mangle]` name)
- `usb_read8` / `usb_read16` / `usb_read32` / `usb_write*` call the Rust helper before `usbctrl_vendorreq`
- Behavior unchanged when `CONFIG_RTL8710B` is disabled (default for 88x2bu)

## Notes

- hal_usb lane follow-up to [W4-25](wave4-25-hal-usb-scaffold.md); still no bulk USB I/O port.
- **Dependency rationale:** extends `rust/hal_usb.rs` introduced in W4-25.

## Acceptance

- L0 build with `CONFIG_RUST` + `CONFIG_USB_HCI`
- L1: fixup symbol present; C read/write paths link against Rust
- L2: host oracle for wvalue below/above `0x0100` with `CONFIG_RTL8710B` enabled in the test compile
