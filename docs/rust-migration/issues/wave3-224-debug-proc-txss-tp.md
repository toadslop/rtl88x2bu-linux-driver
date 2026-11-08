---
title: "[W3-224] Translate rtw_debug_rest.c — proc TXSS by TP leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-224
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port TX spatial-stream control-by-throughput debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_txss.rs`](../../../rust/rtw_debug_proc_txss.rs) (`CONFIG_CTRL_TXSS_BY_TP`):

- `proc_get_txss_tp` / `proc_set_txss_tp`
- `proc_get_txss_ctrl` / `proc_set_txss_ctrl` (`DBG_CTRL_TXSS` only)

## Notes

- Setters touch `mlme_ext_priv` (`txss_ctrl_en`, `txss_tp_th`, `txss_tp_chk_cnt`, `txss_1ss`) only.
- `proc_set_txss_ctrl` calls `dbg_ctrl_txss(adapter, tx_1ss)` — thin FFI; L2 may stub the helper and assert sscanf + mlme field updates.
- Disjoint from MCC and power-save proc Rust modules (parallel lane).

## Acceptance

- L0 with `CONFIG_CTRL_TXSS_BY_TP` (+ `DBG_CTRL_TXSS` for ctrl pair)
- L1 on all four handlers
- L2: enable + threshold vectors; invalid `count` / short sscanf paths
