---
title: "[W3-219] Translate rtw_debug_rest.c — proc PS/WMM + bcn max leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-219
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port power-saving debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ps_info.rs`](../../../rust/rtw_debug_proc_ps_info.rs):

- `proc_get_ps_info` / `proc_set_ps_info` (`CONFIG_POWER_SAVING`)
- `proc_get_wmmps_info` / `proc_set_wmmps_info`
- `proc_get_new_bcn_max` / `proc_set_new_bcn_max` (`CONFIG_BCN_CNT_CONFIRM_HDL`)

## Notes

- Large proc_get_ps_info formatter — split Rust helpers under same module; no new HAL entry points.
- `new_bcn_max` is a global `extern int` — bind via small C shim for L2 host tests.
- Parallel with existing [W3-163](wave3-163-debug-proc-ps-dbg-counters.md) ps lane (disjoint handlers).

## Acceptance

- L0 with relevant Kconfig symbols
- L1 on all six handlers
- L2: ps_info IPS/LPS banner lines for fixture pwrctrl; wmmps set/get; new_bcn_max round-trip
