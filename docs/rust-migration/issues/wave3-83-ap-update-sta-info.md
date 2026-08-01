---
title: "[W3-83] assoc sta info update"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-83
epic: E05
blocked_by: [W3-82]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rust/rtw_ap_rest.rs`](../../../rust/rust/rtw_ap_rest.rs):

- `update_sta_info_apmode`
- `update_sta_info_apmode_ht_bf_cap`
- `update_hw_ht_param`
- `rtw_ap_update_sta_ra_info`

## Notes

- Post-assoc RA/HT updates; W3-73/74 covered IE parse on assoc path.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for assoc sta info update
