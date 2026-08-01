---
title: "[W3-90] sitesurvey cmd handler (backop/complete)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-90
epic: E05
blocked_by: [W3-89]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rust/rtw_mlme_ext_rest.rs`](../../../rust/rust/rtw_mlme_ext_rest.rs):

- `sitesurvey_cmd_hdl`

## Notes

- Part 2: SCAN_BACKING_OP through SCAN_COMPLETE states of sitesurvey_cmd_hdl.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sitesurvey cmd handler (backop/complete)
