---
title: "[W4-53] Translate hal_usb.c — usb_set_intf_ops vtable wiring"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-53
epic: E06
blocked_by: [W4-46]
estimate_loc: 80
---

## Goal

Port `usb_set_intf_ops` from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- Assign `_io_ops` function pointers for USB read/write/port/mem hooks already implemented in C or Rust

## Notes

- Table wiring only — do not move `usb_read_port` / bulk RX in this slice unless already trivial pointer assignment.
- Parallel hal_usb lane root after recv-priv init ([W4-46](wave4-46-hal-usb-init-recv-priv-p2.md)); disjoint from hal_com W4-50 chain.

## Acceptance

- L0 with `CONFIG_USB_HCI` on pinned profile
- L1 symbol for `usb_set_intf_ops`
- Vtable entries match C assignment order and symbols
