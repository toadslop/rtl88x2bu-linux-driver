---
title: "[W3-208] Translate rtw_debug_rest.c — proc VHT 24G enable leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-208
epic: E05
blocked_by: []
estimate_loc: 80
---

## Goal

Port VHT registry debug proc handlers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_vht.rs`](../../../rust/rtw_debug_proc_vht.rs) (`CONFIG_80211AC_VHT`):

- `proc_get_vht_24g_enable` / `proc_set_vht_24g_enable`

## Notes

- Small registry leaf — root of a future **vht proc** lane; `proc_get_dyn_rrsr` deferred (PHYDM).
- Parallel with [W3-203](wave3-203-debug-proc-ht-registry.md) / [W3-207](wave3-207-debug-proc-tx-amsdu.md).

## Acceptance

- L0 with `CONFIG_80211AC_VHT`
- L1 on both handlers
- L2: proc_set mode clamp `< 2`, proc_get prints registry value
