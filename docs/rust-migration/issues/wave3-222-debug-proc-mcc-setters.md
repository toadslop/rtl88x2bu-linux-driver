---
title: "[W3-222] Translate rtw_debug_rest.c — proc MCC registry setters (part 1)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-222
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port multi-channel concurrency debug setters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_mcc.rs`](../../../rust/rtw_debug_proc_mcc.rs) (`CONFIG_MCC_MODE`):

- `proc_set_mcc_enable`
- `proc_set_mcc_duration`
- `proc_set_mcc_phydm_offload_enable`
- `proc_set_mcc_single_tx_criteria`

## Notes

- `proc_get_mcc_info` / `proc_get_mcc_policy_table` call `rtw_hal_dump_mcc_*` — defer getters to a follow-up slice (HAL dump formatters).
- Setters touch `registrypriv` / dvobj iface list only (no MMIO in this batch).
- Root of **mcc proc** lane; TP target setters (`proc_set_mcc_ap_bw*_target_tp`, …) file as W3-223+ later.

## Acceptance

- L0 with `CONFIG_MCC_MODE`
- L1 on all four setters
- L2: enable flag propagated to all iface registry copies; duration sscanf vectors
