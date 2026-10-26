---
title: "[W3-176] Translate rtw_recv.c — RX BH tracking setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-176
epic: E05
blocked_by: [W3-171]
estimate_loc: 120
---

## Goal

Port the DBG_RX_BH_TRACKING state setters from [`core/rtw_recv.c`](../../../core/rtw_recv.c) into
[`rust/rtw_recv_bh_dbg.rs`](../../../rust/rtw_recv_bh_dbg.rs) (same module as [W3-171](wave3-171-recv-rx-bh-tk-dump.md)):

- `rx_bh_tk_set_stage`
- `rx_bh_tk_set_buf`
- `rx_bh_tk_set_buf_pos`
- `rx_bh_tk_set_frame`

## Notes

- Gated on `DBG_RX_BH_TRACKING`; match C `#ifdef` / Kbuild behavior.
- **Dependency rationale:** shares `rust/rtw_recv_bh_dbg.rs` with W3-171 — land dump first.
- Proc/debug call sites stay in C; Rust owns field updates on `struct recv_priv`.

## Acceptance

- L0 + L1 when `DBG_RX_BH_TRACKING` is enabled in the build under test
- L2: host fixture mutates `recv_priv` via setters → `dump_rx_bh_tk` golden snapshot
