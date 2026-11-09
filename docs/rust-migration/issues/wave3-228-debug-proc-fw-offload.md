---
title: "[W3-228] Translate rtw_debug_rest.c — proc FW offload leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-228
epic: E05
blocked_by: []
estimate_loc: 120
---

## Goal

Port FW offload registry procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_fw_offload.rs`](../../../rust/rtw_debug_proc_fw_offload.rs):

- `proc_get_fw_offload` / `proc_set_fw_offload`

## Notes

- Getter prints `RegIQKFWOffload` and `ch_switch_offload` via HAL data FFI.
- Setter may call `rtw_run_in_thread_cmd` + `rtw_hal_update_iqk_fw_offload_cap` when IQK offload toggles — keep thread/cmd dispatch in C.
- Parallel debug lane root (after W3-227 ACK timeout tail).

## Acceptance

- L0 `CONFIG_PROC_DEBUG`
- L1 on both handlers
- L2: sscanf vectors for dual-int setter; getter banner strings with enable/disable
