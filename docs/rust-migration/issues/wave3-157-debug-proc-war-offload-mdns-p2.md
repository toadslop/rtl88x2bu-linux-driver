---
title: "[W3-157] Translate rtw_debug_rest.c — proc war offload MDNS service/txt"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-157
epic: E05
blocked_by: [W3-156]
estimate_loc: 220
---

## Goal

Port deferred MDNS proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_war_offload.rs`](../../../rust/rtw_debug_proc_war_offload.rs) under
`CONFIG_OFFLOAD_MDNS_V4` / `CONFIG_OFFLOAD_MDNS_V6`:

- `proc_get_war_offload_mdns_service_info` / `proc_set_war_offload_mdns_service_info`
- `proc_get_war_offload_mdns_txt_rsp` / `proc_set_war_offload_mdns_txt_rsp`

## Notes

- Deferred from [W3-154](wave3-154-debug-proc-war-offload-mdns-p1.md); reuse `rtw_wow_war_mdns_dump_*` via FFI.
- Preserve `clean` reset path, `txt_idx` bounds checks, and enable-flag gating on getters.
- Chains rtw_debug proc lane after [W3-156](wave3-156-debug-proc-survey-info.md).

## Acceptance

- L0 with war offload + MDNS Kconfig enabled on pinned profile
- L1 on all four symbols
- Service-info and txt_rsp formatting matches C for fixed fixture buffers
