---
title: "[W3-144] Translate rtw_debug_rest.c — dump_adapters_status"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-144
epic: E05
blocked_by: []
estimate_loc: 220
---

## Goal

Port `dump_adapters_status` from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) to
[`rust/rtw_debug_adapters_status.rs`](../../../rust/rtw_debug_adapters_status.rs) under
`CONFIG_RTW_DEBUG` / `CONFIG_PROC_DEBUG`:

- `dump_mi_status` via `extern "C"` to [`core/rtw_mi.c`](../../../core/rtw_mi.c) (`dump_mi_status` /
  `dump_dvobj_mi_status` are still C-only — [W3-122](wave3-122-mi-status-leaf.md) closed without
  porting the dump helpers; a follow-on slice can move them into `rust/rtw_mi.rs`)
- DVobj / multi-BCN / client-port / FW multi-port banners
- Per-iface status table (MAC, mode, channel, security, power, P2P columns)
- DFS / radar_detect status block (`CONFIG_DFS_MASTER`)

## Notes

- Large formatter (~210 LOC in C); keep procfs entry points in C, Rust owns dump body.
- Parallel lane vs [W3-143](wave3-143-debug-mac-bb-reg-dump.md) (different Rust module).
- Split further only if the PR size gate forces it during implementation.

## Acceptance

- L0 with `CONFIG_PROC_DEBUG=y`
- L1 symbol check for `dump_adapters_status`
- Proc debug output matches C baseline on pinned 8822BU config for single-iface STA
