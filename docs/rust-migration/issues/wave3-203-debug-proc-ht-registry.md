---
title: "[W3-203] Translate rtw_debug_rest.c — proc HT registry leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-203
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port HT registry debug proc handlers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ht.rs`](../../../rust/rtw_debug_proc_ht.rs) (`CONFIG_80211N_HT`):

- `proc_get_ht_enable` / `proc_set_ht_enable`
- `proc_get_bw_mode` / `proc_set_bw_mode`
- `proc_get_ampdu_enable` / `proc_set_ampdu_enable`

## Notes

- Registry-only `copy_from_user` + `sscanf` — no HAL/PHYDM calls.
- Parallel **debug** lane root; disjoint from in-flight [W3-202](wave3-202-mi-timer-intf-lifecycle.md) (`rtw_mi.c`).

## Acceptance

- L0 build with proc debug + `CONFIG_80211N_HT` on pinned profile
- L1 symbol parity on all six handlers in the swapped object
- L2: host fixture — set registry fields via proc_set, golden lines from proc_get
