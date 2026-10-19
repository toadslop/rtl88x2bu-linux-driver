---
title: "[W3-143] Translate rtw_debug_rest.c — RTP sink + reorder dump"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-143
epic: E05
blocked_by: []
estimate_loc: 80
---

## Goal

Port recv debug leaf helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) to a new
[`rust/rtw_debug_recv_leaf.rs`](../../../rust/rtw_debug_recv_leaf.rs) (Kbuild object linked when `CONFIG_RUST`):

- `rtw_sink_rtp_seq_dbg`
- `sta_rx_reorder_ctl_dump`

## Notes

- Independent rtw_debug lane (disjoint from `rust/rtw_debug_tx_rx_dumps.rs`); wire via `rtw_debug_rest.c` `#ifndef CONFIG_RUST` guards.
- `rtw_sink_rtp_seq_dbg` is UDP/RTP sequence logging only; reorder dump iterates 16 TIDs.
- L2: host tests with synthetic `sta_info` / `recv_priv` fixtures printing into a buffer.

## Acceptance

- L0 + L1 on swapped symbols
- L2 golden lines for reorder dump (enabled tid) and RTP gap log path
