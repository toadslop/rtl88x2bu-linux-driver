---
title: "[W3-229] Translate rtw_debug_rest.c — proc FW TBTT / BCN offload leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-229
epic: E05
blocked_by: [W3-228]
estimate_loc: 110
---

## Goal

Port BCN/TBTT offload procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_fw_offload.rs`](../../../rust/rtw_debug_proc_fw_offload.rs) (extend module from W3-228):

- `proc_get_fw_tbtt_rpt` / `proc_set_fw_tbtt_rpt` (`CONFIG_FW_HANDLE_TXBCN`)

## Notes

- Setter calls `rtw_hal_set_fw_ap_bcn_offload_cmd` via FFI.
- Getter reads `dvobj->fw_bcn_offload` and `vap_tbtt_rpt_map`.
- Independent of W3-228 at runtime — disjoint proc names; shared Rust file is optional if kbuild stays small.

## Acceptance

- L0 with `CONFIG_FW_HANDLE_TXBCN` on pinned profile
- L1 on both handlers
- L2: dual-arg setter; getter enable/disable + hex map line
