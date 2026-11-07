---
title: "[W3-223] Translate rtw_debug_rest.c — proc MCC target TP setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-223
epic: E05
blocked_by: [W3-222]
estimate_loc: 200
---

## Goal

Port MCC target throughput debug setters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_mcc.rs`](../../../rust/rtw_debug_proc_mcc.rs) (`CONFIG_MCC_MODE`):

- `proc_set_mcc_ap_bw20_target_tp`
- `proc_set_mcc_ap_bw40_target_tp`
- `proc_set_mcc_ap_bw80_target_tp`
- `proc_set_mcc_sta_bw20_target_tp`
- `proc_set_mcc_sta_bw40_target_tp`
- `proc_set_mcc_sta_bw80_target_tp`

## Notes

- Continuation of **mcc proc** lane after [W3-222](wave3-222-debug-proc-mcc-setters.md); stack on W3-222 PR3 wiring when open.
- Each setter assigns one `registrypriv` `u32` on the adapter passed via proc `data`.
- `proc_get_mcc_*` HAL dump getters remain deferred.

## Acceptance

- L0 with `CONFIG_MCC_MODE`
- L1 on all six setters
- L2: sscanf vectors for AP/STA TP fields on registry
