---
title: "[W3-160] Translate rtw_debug_rest.c — proc rate_ctl leaf getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-160
epic: E05
blocked_by: [W3-159]
estimate_loc: 120
---

## Goal

Port rate debug proc **getters** and simple setters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_rate.rs`](../../../rust/rtw_debug_proc_rate.rs):

- `proc_get_rate_ctl`
- `proc_get_bmc_tx_rate` / `proc_set_bmc_tx_rate`
- `proc_get_tx_power_offset` / `proc_set_tx_power_offset`
- `proc_get_bw_ctl` / `proc_set_bw_ctl`

## Notes

- `proc_set_rate_ctl` (PHYDM / fix-rate path) deferred to a follow-up issue — PHYDM coupling.
- `HDATA_RATE` strings via existing `rtw_debug` FFI.
- Chains after [W3-159](wave3-159-debug-proc-trx-info.md).

## Acceptance

- L0 with proc debug on pinned profile
- L1 on all symbols in this slice
- Fixed-rate and BMC rate lines match C for fixture adapter state
