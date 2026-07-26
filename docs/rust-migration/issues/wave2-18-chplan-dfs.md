---
title: "[W2-18] Translate rtw_chplan DFS + exclusion helpers"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-18
epic: E04
blocked_by: [W2-17]
estimate_loc: 200
---

## Goal

Port DFS and exclusion helpers from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c) to [`rust/rtw_chplan.rs`](../../../rust/rtw_chplan.rs):

- `rtw_regsty_is_excl_chs`
- `rtw_chset_is_dfs_range`, `rtw_chset_is_dfs_ch`, `rtw_chset_is_dfs_chbw`

## Table strategy

Same as W2-17: static tables remain in C; Rust accesses channel-plan data via `extern "C"`.

## Notes

- `rtw_chbw_to_freq_range` / `rtw_freq2ch` may remain C calls through thin FFI until `rtw_wlan_util` channel helpers land.
- L2: table-driven DFS range tests with synthetic `RT_CHANNEL_INFO` channel sets (extend T4 harness).
- In-flight: `cursor/w2-18-chplan-dfs-rust-3dd4`.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T4)
