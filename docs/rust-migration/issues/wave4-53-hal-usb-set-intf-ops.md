---
title: "[W4-53] Extend rust/hal_usb.rs usb_set_intf_ops — interrupt pipe + hook retarget"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-53
epic: E06
blocked_by: [W4-46]
estimate_loc: 60
---

## Goal

**Follow-up to [W4-25](wave4-25-hal-usb-scaffold.md)** — the initial `usb_set_intf_ops` vtable fill already lives in
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs) (`HAL_USB_SET_INTF_OPS_IN_RUST` in
[`hal/hal_hci/hal_usb_rest.c`](../../../hal/hal_hci/hal_usb_rest.c)). This slice closes the gap vs the C
reference in [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c):

- Wire `_read_interrupt` when `CONFIG_USB_INTERRUPT_IN_PIPE` (C assigns `usb_read_interrupt`; Rust table omits it today)
- As hal_usb lane slices land more I/O in Rust, retarget any remaining `_io_ops` slots from C symbols to the Rust exports (no duplicate “first port” of `usb_set_intf_ops`)

## Notes

- **Not** a re-port of W4-25 — do not reintroduce C `usb_set_intf_ops` or change L1 symbol ownership.
- Table wiring only — do not move `usb_read_port` / bulk RX bodies in this slice.
- Chains hal_usb lane after recv-priv init ([W4-46](wave4-46-hal-usb-init-recv-priv-p2.md)); disjoint from hal_com W4-50 chain.

## Acceptance

- L0 with `CONFIG_USB_HCI` on pinned profile (including interrupt-pipe profile if enabled in Kconfig)
- L1: `usb_set_intf_ops` unchanged symbol; vtable matches C assignment order for all hooks this slice owns
- Host oracle or L2 harness delta if interrupt-pipe wiring is testable without hardware
