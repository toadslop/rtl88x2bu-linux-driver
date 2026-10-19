---
title: "[W4-43] Translate hal_usb.c — usb_free_recv_priv teardown"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-43
epic: E06
blocked_by: [W4-37]
estimate_loc: 200
---

## Goal

Port `usb_free_recv_priv` from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- Free `recv_buf` queue entries and OS recvbuf resources
- `CONFIG_USB_INTERRUPT_IN_PIPE` urb/buffer cleanup
- `CONFIG_PREALLOC_RECV_SKB` / `free_recv_skb_queue` drain
- Linux `rx_skb_queue` / `free_recv_skb_queue` purge and prealloc-skb drain paths
- FreeBSD skb dequeue and `CONFIG_RX_INDICATE_QUEUE` mbuf drain + `mtx_destroy`

## Notes

- Complements init in `usb_init_recv_priv` (stays in C for this slice unless trivial).
- Chains hal_usb lane after async vendor writes [W4-37](wave4-37-hal-usb-async-vendor-write.md).
- Kernel-only paths; L0 + L1 primary gates (no host L2 unless a thin mock harness is added).

## Acceptance

- L0 build with USB HCI enabled
- L1 symbol parity for `usb_free_recv_priv`
- `extern "C"` signature unchanged for `usb_dvobj_deinit` callers
