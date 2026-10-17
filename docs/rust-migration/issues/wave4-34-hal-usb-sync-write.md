---
title: "[W4-34] Translate hal_usb.c — sync vendor write8/16/32/N"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-34
epic: E06
blocked_by: [W4-33]
estimate_loc: 200
---

## Goal

Port synchronous USB vendor write helpers from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- `usb_write8`, `usb_write16`, `usb_write32`, `usb_writeN` (including `VENDOR_CMD_MAX_DATA_LEN` stack buffer copy for `writeN`)
- Same request/wvalue/fixup pattern as reads; `requesttype` write_out

## Notes

- Same-file follow-up to [W4-33](wave4-33-hal-usb-sync-read.md).
- `usb_set_intf_ops` already points at C write symbols today — retarget to Rust after L1 passes.

## Acceptance

- L0 + L1 for all four write symbols on `rust/hal_usb.o`
- L2: host oracle for 1/2/4-byte writes and bounded `writeN` payload
