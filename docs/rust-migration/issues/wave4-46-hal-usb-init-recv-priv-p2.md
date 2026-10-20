---
title: "[W4-46] Translate hal_usb.c — usb_init_recv_priv (part 2/2)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-46
epic: E06
blocked_by: [W4-45]
estimate_loc: 200
---

## Goal

Port the **second half** of `usb_init_recv_priv` from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- Per-`recv_buf` list init, `rtw_os_recvbuf_resource_alloc` loop, and `free_recv_buf_queue_cnt`
- `rx_skb_queue` / `CONFIG_RX_INDICATE_QUEUE` init (Linux/FreeBSD blocks)
- `CONFIG_PREALLOC_RECV_SKB` alignment + `skb_queue_tail` loop
- Export full `usb_init_recv_priv` from Rust; C body guarded by `HAL_USB_INIT_RECV_PRIV_IN_RUST`

## Notes

- Depends on part 1 queue/buffer setup ([W4-45](wave4-45-hal-usb-init-recv-priv-p1.md)).
- Preserve `_FAIL` mid-loop break behavior and `exit` label cleanup semantics.

## Acceptance

- L0 + L1 symbol parity for `usb_init_recv_priv`
- L3: module load/unload in QEMU unchanged (no new Oops on USB probe path)
