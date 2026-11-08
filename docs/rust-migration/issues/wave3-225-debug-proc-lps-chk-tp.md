---
title: "[W3-225] Translate rtw_debug_rest.c — proc LPS check-by-TP leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-225
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port LPS throughput-threshold debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_lps_tp.rs`](../../../rust/rtw_debug_proc_lps_tp.rs) (`CONFIG_LPS_CHK_BY_TP`):

- `proc_get_lps_chk_tp` / `proc_set_lps_chk_tp`

## Notes

- Setter/getter read and update `pwrctrl_priv` LPS-by-TP fields (`lps_chk_by_tp`, `lps_*_tp_th`, counters, packet thresholds).
- Multi-field `sscanf` with partial-argument semantics match C (enable required; thresholds optional).
- Parallel with TXSS and MCC proc lanes; no shared Rust module files.

## Acceptance

- L0 with `CONFIG_LPS_CHK_BY_TP`
- L1 on both handlers
- L2: fixture `pwrctrl_priv` oracle for enable + threshold propagation vectors
