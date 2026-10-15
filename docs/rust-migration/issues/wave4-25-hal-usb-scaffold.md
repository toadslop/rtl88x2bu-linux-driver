---
title: "[W4-25] Kbuild scaffold — rust/hal_usb.rs + hal_usb_rest.c split"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-25
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Introduce [`rust/hal_usb.rs`](../../../rust/hal_usb.rs) and wire Kbuild so `hal/hal_hci/hal_usb.c`
can shrink to [`hal/hal_hci/hal_usb_rest.c`](../../../hal/hal_hci/hal_usb_rest.c) using the same
`*_rest.c` pattern as `hal_com` / `hal_phy`:

- Add `rust/hal_usb.o` to the module link when `CONFIG_RUST` is enabled
- Move `usb_set_intf_ops` into Rust (function-pointer table fill only; I/O ops remain C for now)
- Extract unported `hal_usb.c` symbols into `hal_usb_rest.c`

## Notes

- Parallel **hal_usb** lane — disjoint from `rust/hal_halmac.rs` and `rust/hal_phy.rs`.
- No `usb_read*` / `usb_write*` port in this slice (MMIO/USB stack stays in C).
- `CONFIG_USB_HCI` must match existing `hal_$(HCI_NAME).o` selection in the root `Makefile`.

## Acceptance

- L0 build links `rust/hal_usb.o` + `hal/hal_hci/hal_usb_rest.o` under `CONFIG_RUST` + `CONFIG_USB_HCI`
- L1: `usb_set_intf_ops` symbol present and callable from C init
- L3: QEMU insmod/rmmod unchanged (no Oops on module load)
