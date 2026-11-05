---
title: "[W3-221] Translate rtw_debug_rest.c — proc TX mgmt inject test leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-221
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port manufacturing / test TX management procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_tx_mgmt.rs`](../../../rust/rtw_debug_proc_tx_mgmt.rs) (`CONFIG_AP_MODE` / debug builds):

- `proc_set_tx_sa_query`
- `proc_set_tx_deauth`
- `proc_set_tx_auth`

## Notes

- Handlers parse MAC + reason codes then call existing C TX helpers — thin FFI only in Rust.
- No proc_get counterparts; L2 focuses on sscanf validation and early `-EINVAL` paths.
- Disjoint from [W3-220](wave3-220-debug-proc-monitor-xmit.md) Rust module.

## Acceptance

- L0 on AP-capable profile
- L1 on all three setters
- L2: invalid parameter vectors; well-formed MAC line reaches stub TX hook (record call args)
