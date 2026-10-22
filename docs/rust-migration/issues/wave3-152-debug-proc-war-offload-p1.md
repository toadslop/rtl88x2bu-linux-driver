---
title: "[W3-152] Translate rtw_debug_rest.c — proc war offload enable + IPv4"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-152
epic: E05
blocked_by: [W3-151]
estimate_loc: 180
---

## Goal

Port `CONFIG_WAR_OFFLOAD` proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_get_war_offload_enable`
- `proc_set_war_offload_enable`
- `proc_set_war_offload_ipv4_addr`

## Notes

- Mirror optional `CONFIG_OFFLOAD_MDNS_*` banner columns with Rust `cfg` attributes (same as C `#ifdef`).
- Chains rtw_debug proc lane after [W3-151](wave3-151-debug-proc-scan-param.md); C swap guards match W3-145…151.

## Acceptance

- L0 with `CONFIG_WAR_OFFLOAD=y` on pinned profile
- L1 on all three proc symbols
- `sscanf` / `copy_from_user` validation paths match C
