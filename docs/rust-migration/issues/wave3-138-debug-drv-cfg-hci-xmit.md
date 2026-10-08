---
title: "[W3-138] Translate rtw_debug.c — dump_drv_cfg HCI + xmit/recv banners"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-138
epic: E05
blocked_by: [W3-131]
estimate_loc: 200
---

## Goal

Port **part 2** of `dump_drv_cfg` from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs) (under `CONFIG_PROC_DEBUG`):

- `CONFIG_USB_HCI` / `CONFIG_SDIO_HCI` / `CONFIG_PCI_HCI` conditional banners
  (C ~L173–220).
- Interface count / MBSSID / TXBCN options and **XMIT-INFO** / **RECV-INFO** sections
  (C ~L222–252), including `rtw_recvbuf_nr` FFI read.

## Notes

- Completes `dump_drv_cfg` started in `W3-131`; same `extern "C"` entry point.
- USB path is active on the pinned 8822BU config; SDIO/PCI branches stay behind `cfg`.
- Helper functions in Rust OK if needed for reviewability.

## Acceptance

- L0 build with `CONFIG_PROC_DEBUG=y`
- L1 symbol parity for `dump_drv_cfg`
- Proc debug output matches C baseline for HCI + xmit/recv sections on pinned config
