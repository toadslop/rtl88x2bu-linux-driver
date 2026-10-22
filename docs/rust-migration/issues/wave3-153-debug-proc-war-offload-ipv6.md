---
title: "[W3-153] Translate rtw_debug_rest.c — proc war offload IPv6 addr"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-153
epic: E05
blocked_by: [W3-152]
estimate_loc: 120
---

## Goal

Port from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_set_war_offload_ipv6_addr` — index + eight `hx` tuple, `htons` + `_rtw_memcpy` into `wowlan_war_offload_ipv6`

## Notes

- Requires [W3-152](wave3-152-debug-proc-war-offload-p1.md) guard/Makefile lane on `rtw_debug_rest.c`.
- Getter for IPv6 is not present in C today — do not add new proc surface.

## Acceptance

- L0 with `CONFIG_WAR_OFFLOAD=y`
- L1 on `proc_set_war_offload_ipv6_addr`
- Wrong `sscanf` field count returns `-EFAULT` like C
