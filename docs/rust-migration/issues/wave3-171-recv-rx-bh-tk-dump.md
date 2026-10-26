---
title: "[W3-171] Translate rtw_recv.c — RX BH tracking dump leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-171
epic: E05
blocked_by: []
estimate_loc: 60
---

## Goal

Port the DBG_RX_BH_TRACKING debug formatter from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to
[`rust/rtw_recv_bh_dbg.rs`](../../../rust/rtw_recv_bh_dbg.rs):

- `dump_rx_bh_tk`

## Notes

- Gated on `DBG_RX_BH_TRACKING` (same as C); preserve Kbuild/cfg behavior.
- Used from proc/debug paths in `rtw_debug.c` / `rtw_debug_rest.c` — keep call sites in C.
- Independent recv lane (no overlap with debug reg dump modules).

## Acceptance

- L0 + L1 when `DBG_RX_BH_TRACKING` is enabled in the build under test
- L2: minimal `recv_priv` fixture → single golden line
