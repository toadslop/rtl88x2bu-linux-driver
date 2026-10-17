---
title: "[W4-37] Translate hal_usb.c — async vendor write helpers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-37
epic: E06
blocked_by: [W4-34]
estimate_loc: 160
---

## Goal

Port `CONFIG_USB_SUPPORT_ASYNC_VDN_REQ` helpers from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- `usb_write_async`, `usb_async_write8`, `usb_async_write16`, `usb_async_write32`
- Preserve vendor request constants and `_usbctrl_vendorreq_async_write` delegation

## Notes

- Optional Kconfig path — host L2 compiles with `CONFIG_USB_SUPPORT_ASYNC_VDN_REQ` enabled.
- Chains hal_usb lane after sync writes ([W4-34](wave4-34-hal-usb-sync-write.md)).

## Acceptance

- L0 with async VDN Kconfig on
- L1 symbols exported from `rust/hal_usb.o`
- L2: mocked async vendorreq records addr/width payload
