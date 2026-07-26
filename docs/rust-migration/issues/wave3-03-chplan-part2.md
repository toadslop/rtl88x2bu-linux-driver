---
title: "[W3-03] Translate rtw_chplan.c part 2 — excl_chs + DFS helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-03
epic: E05
blocked_by: [W3-02]
estimate_loc: 200
---

## Goal

Port from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c) (or `rtw_chplan_rest.c` after W3-02):

- `rtw_regsty_is_excl_chs`
- `rtw_chset_is_dfs_range`, `rtw_chset_is_dfs_ch`, `rtw_chset_is_dfs_chbw`

## Notes

- `rtw_chbw_to_freq_range` / `rtw_freq2ch` may remain C calls through thin FFI until `rtw_wlan_util` rate/channel helpers land.
- L2: table-driven DFS range tests with synthetic `RT_CHANNEL_INFO` channel sets.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T4)
