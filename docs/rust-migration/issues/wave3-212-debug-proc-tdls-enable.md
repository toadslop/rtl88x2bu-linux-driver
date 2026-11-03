---
title: "[W3-212] Translate rtw_debug_rest.c — proc TDLS enable + function info"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-212
epic: E05
blocked_by: [W3-208]
estimate_loc: 200
---

## Goal

Port TDLS debug proc entry points from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_tdls.rs`](../../../rust/rtw_debug_proc_tdls.rs) (`CONFIG_TDLS`):

- `proc_get_tdls_enable` / `proc_set_tdls_enable`
- `proc_tdls_display_tdls_function_info` (static helper; export for seq_file wrapper)

## Notes

- Setters call `rtw_enable_tdls_func` / `rtw_disable_tdls_func` via thin C FFI (TDLS state machine stays in C).
- Parallel root with [W3-209](wave3-209-debug-proc-en-fwps.md) after W3-208 — disjoint Rust module from fwps/phy lanes.
- Channel-switch dump lines inside the helper remain behind `CONFIG_TDLS_CH_SW`.

## Acceptance

- L0 with `CONFIG_TDLS`
- L1 on enable procs + display helper symbol used by procfs
- L2: enable proc golden strings; function-info header lines for fixture `tdlsinfo`
