---
title: "[W3-218] Translate rtw_debug_rest.c — proc WoWLAN pattern leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-218
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port WoWLAN wake-pattern debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_wow_pattern.rs`](../../../rust/rtw_debug_proc_wow_pattern.rs) (`CONFIG_WOWLAN`):

- `proc_get_pattern_info`
- `proc_set_pattern_info` (`clean` subcommand + `rtw_wowlan_parser_pattern_cmd` path)

## Notes

- `proc_set_pattern_info` may call `rtw_hal_set_hwreg(HW_VAR_WOWLAN, …)` on `clean` — stub HAL in L2 oracle or gate L2 to parser-only paths.
- Shares `pwrctrl_priv` pattern arrays with C; Rust formats hex dumps for proc_get.
- Parallel with [W3-217](wave3-217-debug-proc-wow-registry.md) wow lane (disjoint `rtw_debug_proc_wow*.rs`
  modules; no `blocked_by` — pattern procs do not require registry handlers to land first).

## Acceptance

- L0 with `CONFIG_WOWLAN`
- L1 on both handlers
- L2: proc_get empty pattern table; proc_set parser success increments `wowlan_pattern_idx` (fixture pwrctrl)
