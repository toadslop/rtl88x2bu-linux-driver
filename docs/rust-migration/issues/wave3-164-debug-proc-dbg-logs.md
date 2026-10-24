---
title: "[W3-164] Translate rtw_debug_rest.c — proc dbg counter logs"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-164
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port `CONFIG_DBG_COUNTER` proc log dumpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_dbg_logs.rs`](../../../rust/rtw_debug_proc_dbg_logs.rs):

- `proc_get_rx_logs`
- `proc_get_tx_logs`
- `proc_get_int_logs`

## Notes

- Large `RTW_PRINT_SEL` format blocks — port verbatim field order and labels.
- Touches `adapter->rx_logs`, `adapter->tx_logs`, `adapter->int_logs` only (read-only getters).

## Acceptance

- L0 + L1 when `CONFIG_DBG_COUNTER` is enabled
- Snapshot of output matches C for a fixture adapter with non-zero log counters
