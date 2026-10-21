---
title: "[W3-150] Translate rtw_debug_rest.c — proc rx/tx stainfo stats"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-150
epic: E05
blocked_by: [W3-149]
estimate_loc: 200
---

## Goal

Port stainfo statistics proc getters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_get_rx_stat` — walk `sta_hash` under `sta_hash_lock`, per-sta RX counters
- `proc_get_tx_stat` — same lock discipline for TX counters / queue stats

## Notes

- Preserve `_enter_critical_bh` / `_exit_critical_bh` pairing and BC/null MAC filtering behavior from C.
- Chains after [W3-149](wave3-149-debug-proc-defs-reg.md) on `rtw_debug_proc.rs`.

## Acceptance

- L0 with `CONFIG_PROC_DEBUG=y`
- L1 symbols for both `proc_get_*` handlers
- Output column layout matches C (spot-check via proc seq_file or unit harness if added later)
