---
title: "[W3-226] Translate rtw_debug_rest.c — proc static SMPS leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-226
epic: E05
blocked_by: []
estimate_loc: 190
---

## Goal

Port static SMPS debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_smps.rs`](../../../rust/rtw_debug_proc_smps.rs) (`CONFIG_SUPPORT_STATIC_SMPS`):

- `proc_get_smps` / `proc_set_smps`

## Notes

- Setter parses `sscanf` input and updates `mlme_ext_priv` (`ssmps_en`, `ssmps_tx_tp_th`,
  `ssmps_rx_tp_th`; when `DBG_STATIC_SMPS`, optional `ssmps_test` / `ssmps_test_en`) — no
  workqueue/FFI side effects in C.
- Getter prints enable/TP thresholds (and optional DBG test fields) from `mlme_ext_priv` only —
  format strings unchanged.
- Parallel lane (disjoint `rust/rtw_debug_proc_smps.rs`).

## Acceptance

- L0 with `CONFIG_SUPPORT_STATIC_SMPS`
- L1 on both handlers
- L2: sscanf vectors for enable/threshold fields (and `DBG_STATIC_SMPS` extras when enabled)
