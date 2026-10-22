---
title: "[W3-154] Translate rtw_debug_rest.c — proc war offload MDNS domain/machine"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-154
epic: E05
blocked_by: [W3-153]
estimate_loc: 200
---

## Goal

Port MDNS proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) under
`CONFIG_OFFLOAD_MDNS_V4` / `CONFIG_OFFLOAD_MDNS_V6`:

- `proc_get_war_offload_mdns_domain_name` / `proc_set_war_offload_mdns_domain_name`
- `proc_get_war_offload_mdns_machine_name` / `proc_set_war_offload_mdns_machine_name`

## Notes

- Preserve enable-flag gating on getters (`wowlan_war_offload_mode` + `wowlan_war_offload_ctrl` bits).
- Service-info / txt_rsp slices deferred to a follow-up issue in the same lane.

## Acceptance

- L0 with war offload + MDNS Kconfig enabled on pinned profile
- L1 on all four symbols
- Domain/machine string length caps match `MAX_MDNS_*` constants
