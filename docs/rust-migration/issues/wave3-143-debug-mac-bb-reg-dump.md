---
title: "[W3-143] Translate rtw_debug_rest.c — MAC/BB register dump leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-143
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port register dump formatters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) to
[`rust/rtw_debug_reg_dump.rs`](../../../rust/rtw_debug_reg_dump.rs) (Kbuild object when
`CONFIG_RUST` and `CONFIG_RUST_RTW_DEBUG`):

- `mac_reg_dump`
- `bb_reg_dump`

## Notes

- **Not in scope:** `rtw_sink_rtp_seq_dbg` / `sta_rx_reorder_ctl_dump` — already shipped under
  closed [W3-134](wave3-134-debug-tx-rx-dumps.md) in `rust/rtw_debug_tx_rx_dumps.rs` with C guards in
  `core/rtw_debug_rest.c`.
- Reads use existing `rtw_read32` FFI; procfs walkers and `rf_reg_dump` stay in C for later slices
  (see [W3-131](wave3-131-debug-drv-cfg-dump.md)).
- Chip-specific `#ifdef` ranges follow the C source; pin acceptance on 8822BU (`CONFIG_RTL8822B`).
- Wire via `rtw_debug_rest.c` `#if !defined(CONFIG_RUST) || !defined(CONFIG_RUST_RTW_DEBUG)` guards
  (same pattern as W3-134).

## Acceptance

- L0 + L1 on swapped symbols
- Optional L2: golden lines for a fixed `rtw_read32` stub table on 8822BU MAC/BB windows
