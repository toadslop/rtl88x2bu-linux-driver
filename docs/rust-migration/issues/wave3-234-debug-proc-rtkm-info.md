---
title: "[W3-234] Translate rtw_debug_rest.c — RTKM prealloc buffer info proc"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-234
epic: E05
blocked_by: []
estimate_loc: 80
---

## Goal

Port RTKM diagnostics proc from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_rtkm.rs`](../../../rust/rtw_debug_proc_rtkm.rs):

- `proc_get_rtkm_info` (`CONFIG_PREALLOC_RX_SKB_BUFFER`)

## Notes

- USB path prints `recv_buf->alloc_sz`; calls `rtw_rtkm_get_nr_recv_skb` / `rtw_rtkm_get_buff_size`.
- High value for **88x2bu** (USB HCI) — parallel root unrelated to PCI lanes.
- Getter-only slice.

## Acceptance

- L0 with `CONFIG_PREALLOC_RX_SKB_BUFFER` + `CONFIG_USB_HCI`
- L1 on handler
- L2: banner lines for RTKM vs driver buffer constants
