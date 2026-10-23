---
title: "[W3-158] Translate rtw_debug_rest.c — proc ap_info + infra_ap"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-158
epic: E05
blocked_by: [W3-157]
estimate_loc: 200
---

## Goal

Add [`rust/rtw_debug_proc_ap.rs`](../../../rust/rtw_debug_proc_ap.rs) (Kbuild object when `CONFIG_RUST` and
`CONFIG_RUST_RTW_DEBUG`) and port proc seq_file getters from
[`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c):

- `proc_get_ap_info` — associated AP / STA link dump (`CONFIG_PROC_DEBUG`)
- `proc_get_infra_ap` — Roku infra-AP HT/VHT columns (`ROKU_PRIVATE`)

## Notes

- `wireless_mode_to_str` helper moves with `proc_get_ap_info`; keep output strings identical.
- `get_assoc_AP_Vendor` / `sta_rx_reorder_ctl_dump` stay in C via thin FFI.
- Chains after [W3-157](wave3-157-debug-proc-war-offload-mdns-p2.md) on `rtw_debug_rest.c` guard lane.

## Acceptance

- L0 with proc debug enabled on pinned profile
- L1 on both proc symbols
- AP info lines match C for a fixed STA fixture (manual or host shim)
