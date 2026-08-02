---
title: "[W3-106] rson score/IE/choose leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-106
epic: E05
blocked_by: [W3-105]
estimate_loc: 194
---

## Goal

Port helpers from [`core/rtw_rson.c`](../../../core/rtw_rson.c) to [`rust/rtw_rson.rs`](../../../rust/rtw_rson.rs):

- `rtw_cal_rson_score`
- `is_match_bssid`
- `init_rtw_rson_data`
- `str2hexbuf`
- `rtw_rson_varify_ie`
- `rtw_get_rson_struct`
- `rtw_rson_choose`
- `rtw_rson_append_ie`

## Notes

- Low-coupling roaming score/IE/choose leaf helpers on `rtw_rson.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for rson score/IE/choose leaf helpers
