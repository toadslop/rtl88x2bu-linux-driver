---
title: "[W3-162] Translate rtw_debug_rest.c — proc rx counter dump mode"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-162
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port RX counter dump proc hooks from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_rx_cnt.rs`](../../../rust/rtw_debug_proc_rx_cnt.rs) under `DBG_RX_COUNTER_DUMP`:

- `proc_get_rx_cnt_dump`
- `proc_set_rx_cnt_dump`

## Notes

- Setter calls `rtw_dump_phy_rxcnts_preprocess` before updating `adapter->dump_rx_cnt_mode` — keep call order.
- Independent debug proc lane (new Rust object); no overlap with `rtw_debug_proc_rate.rs`.

## Acceptance

- L0 + L1 on both symbols when `DBG_RX_COUNTER_DUMP` is enabled on the build profile
- Getter banner text and mode print unchanged
