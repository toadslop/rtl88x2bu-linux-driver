---
title: "[W3-89] sitesurvey cmd handler (enter/process)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-89
epic: E05
blocked_by: [W3-88]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `sitesurvey_cmd_hdl`

## Notes

- Part 1: SCAN_DISABLE through SCAN_PROCESS states of sitesurvey_cmd_hdl.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sitesurvey cmd handler (enter/process)
