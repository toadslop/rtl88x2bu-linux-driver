---
title: "[W3-235] Translate rtw_debug_rest.c — WoWLAN keep-alive pattern dump"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-235
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port WoWLAN keep-alive debug proc from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_wow_keepalive.rs`](../../../rust/rtw_debug_proc_wow_keepalive.rs):

- `proc_dump_wow_keep_alive_info` (`CONFIG_WOWLAN` + `CONFIG_WOW_KEEP_ALIVE_PATTERN`)

## Notes

- Dumps `pwrctrl_priv` keep-alive fields and hex pattern bytes (8-wide rows).
- No setter — read-only proc.
- Parallel WoWLAN lane distinct from W3-217…W3-218 registry/pattern setters.

## Acceptance

- L0 with WoWLAN Kconfig symbols on pinned profile
- L1 on handler
- L2: pattern length + period lines with stub pwrctrl fixture
