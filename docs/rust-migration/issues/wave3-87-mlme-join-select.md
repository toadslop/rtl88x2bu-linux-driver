---
title: "[W3-87] join candidate select"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-87
epic: E05
blocked_by: [W3-86]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rust/rtw_mlme_rest.rs`](../../../rust/rust/rtw_mlme_rest.rs):

- `rtw_select_and_join_from_scanned_queue`
- `_rtw_check_join_candidate`
- `_rtw_sitesurvey_condition_check`

## Notes

- FSM entry before join_cmd_hdl; W3-53..67 covered leaf mlme helpers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for join candidate select
