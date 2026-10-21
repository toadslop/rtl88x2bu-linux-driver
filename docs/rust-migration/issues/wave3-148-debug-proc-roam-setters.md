---
title: "[W3-148] Translate rtw_debug_rest.c — proc roam setters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-148
epic: E05
blocked_by: [W3-147]
estimate_loc: 90
---

## Goal

Port remaining `CONFIG_LAYER2_ROAMING` proc setters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_set_roam_param` — `copy_from_user` + partial `sscanf` updates to `mlmepriv` roam fields
- `proc_set_roam_tgt_addr` — MAC parse + `roam_tgt_addr` copy

## Notes

- Serial follow-up on the W3-145…147 `rtw_debug_proc.rs` lane (same Makefile object / `CONFIG_RUST_RTW_DEBUG` guards).
- [W3-147](wave3-147-debug-proc-roam-leaf.md) already ported the matching getters + `proc_set_roam_flags`.

## Acceptance

- L0 with `CONFIG_LAYER2_ROAMING=y` and `CONFIG_PROC_DEBUG=y` on pinned profile
- L1 symbol parity on the two `proc_set_*` handlers in the swapped object
- Preserve `-EFAULT`, buffer length checks, and optional-field `sscanf` semantics
