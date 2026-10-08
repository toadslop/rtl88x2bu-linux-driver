---
title: "[W3-134] Translate rtw_debug.c — tx/rx debug dump leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-134
epic: E05
blocked_by: [W3-130]
estimate_loc: 200
---

## Goal

Port tx/rx debug helpers from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs):

- `rtw_sink_rtp_seq_dbg`
- `sta_rx_reorder_ctl_dump`
- `dump_tx_rate_bmp`

## Notes

- Touches `sta_info`, `dvobj_priv`, and rate bitmap fields already typed in domain/Rust
  recv/xmit modules — reuse existing struct layouts from bindgen/ffi.
- Heavy procfs walkers remain in C.

## Acceptance

- L0 + L1
- L2 optional with minimal `sta_info` / `dvobj_priv` fixtures
