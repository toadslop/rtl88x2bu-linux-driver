---
title: "[W4-45] Translate hal_usb.c — usb_init_recv_priv (part 1/2)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-45
epic: E06
blocked_by: [W4-44]
estimate_loc: 180
---

## Goal

Port the **first half** of `usb_init_recv_priv` from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- Linux `tasklet_init` for `recv_tasklet` (`CONFIG` / platform gated as in C)
- `CONFIG_USB_INTERRUPT_IN_PIPE` urb + `int_in_buf` allocation and failure paths
- Init `free_recv_buf_queue`, `recv_buf_pending_queue`, and `free_recv_skb_queue` (non-buffer-alloc RX)
- Allocate `pallocated_recv_buf` / align `precv_buf`; early `_FAIL` cleanup labels

## Notes

- Part 2 ([W4-46](wave4-46-hal-usb-init-recv-priv-p2.md)) covers the per-`recv_buf` loop, prealloc SKB queue, and success exit.
- Mirror `hal_usb_rest.c` layout offsets pattern from [W4-43](wave4-43-hal-usb-free-recv-priv.md) for any new `recv_priv` fields touched.
- Complements existing Rust teardown in `usb_free_recv_priv`.

## Acceptance

- L0 USB HCI build (pinned profile)
- L1: symbols for any new `extern "C"` helpers; `usb_init_recv_priv` may remain C stub calling Rust until part 2 lands
- No regression in `usb_free_recv_priv` pairing assumptions
