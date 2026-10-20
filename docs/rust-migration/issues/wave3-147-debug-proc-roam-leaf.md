---
title: "[W3-147] Translate rtw_debug_rest.c — proc roam debug leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-147
epic: E05
blocked_by: [W3-146]
estimate_loc: 160
---

## Goal

Port `CONFIG_LAYER2_ROAMING` proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) (module introduced in [W3-145](wave3-145-debug-proc-mlme-leaf.md)):

- `proc_get_roam_flags`
- `proc_get_roam_param` (table header + `mlme_priv` roam fields)
- `proc_set_roam_flags` — `copy_from_user` + `rtw_assign_roam_flags`

## Notes

- Serial follow-up after [W3-146](wave3-146-debug-proc-sec-info.md) on `rust/rtw_debug_proc.rs` (same
  Makefile object / guard lane as W3-145/146).
- Kconfig-gated same as C (`CONFIG_LAYER2_ROAMING`).

## Acceptance

- L0 with `CONFIG_LAYER2_ROAMING=y` and `CONFIG_PROC_DEBUG=y` on pinned profile
- L1 symbol parity on roam `proc_get_*` / `proc_set_*` in the swapped object (not `EXPORT_SYMBOL`);
  set handler preserves `-EFAULT` / sscanf semantics
