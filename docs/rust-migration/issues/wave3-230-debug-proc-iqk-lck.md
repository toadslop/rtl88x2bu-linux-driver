---
title: "[W3-230] Translate rtw_debug_rest.c — proc IQK / LCK cal test leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-230
epic: E05
blocked_by: []
estimate_loc: 100
---

## Goal

Port RF calibration test procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_cal.rs`](../../../rust/rtw_debug_proc_cal.rs):

- `proc_get_iqk_info` / `proc_set_iqk`
- `proc_get_lck_info` / `proc_set_lck`

(`CONFIG_DBG_RF_CAL`)

## Notes

- C getters are stubs returning 0 — preserve behavior.
- Setters parse user buffer then call `rtw_hal_iqk_test` / `rtw_hal_lck_test` through FFI (no new MMIO in Rust).
- Parallel lane — different Rust module than W3-228/W3-229.

## Acceptance

- L0 with `CONFIG_DBG_RF_CAL`
- L1 on all four handlers
- L2: invalid sscanf arity on iqk setter; single-int lck trigger path
