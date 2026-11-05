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

- `proc_get_tx_sa_query` / `proc_set_tx_sa_query`
- `proc_get_tx_deauth` / `proc_set_tx_deauth`
- `proc_get_tx_auth` / `proc_set_tx_auth`

## Notes

- Setters parse MAC + reason codes then call existing C TX helpers — thin FFI only in Rust.
- Getters print `__func__` only (no adapter state); include in L1 for symbol parity, L2 optional smoke.
- L2 focuses on setter sscanf validation and early `-EINVAL` paths.
- Disjoint from [W3-220](wave3-220-debug-proc-monitor-xmit.md) Rust module.

## Acceptance

- L0 on AP-capable profile
- L1 on all six handlers
- L2: invalid parameter vectors on setters; well-formed MAC line reaches stub TX hook (record call args)
