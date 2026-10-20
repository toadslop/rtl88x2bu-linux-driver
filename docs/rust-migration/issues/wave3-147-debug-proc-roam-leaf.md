---
title: "[W3-147] Translate rtw_debug_rest.c — proc roam debug leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-147
epic: E05
blocked_by: [W3-145]
estimate_loc: 160
---

## Goal

Port `CONFIG_LAYER2_ROAMING` proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) (module introduced in [W3-145](wave3-145-debug-proc-mlme-leaf.md)):

- `proc_get_roam_flags`
- `proc_get_roam_param` (table header + `mlme_priv` roam fields)
- `proc_set_roam_flags` — `copy_from_user` + `rtw_assign_roam_flags`

## Notes

- Chains the debug proc lane after [W3-145](wave3-145-debug-proc-mlme-leaf.md) (`rtw_debug_proc.rs` scaffold).
- Kconfig-gated same as C.

## Acceptance

- L0 with `CONFIG_LAYER2_ROAMING=y` on pinned profile
- L1 symbols; set handler preserves `-EFAULT` / sscanf semantics
