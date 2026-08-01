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

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `sitesurvey_cmd_hdl`

## Notes

- **`sitesurvey_cmd_hdl` (~417 LOC total)** split at FSM boundary: part 2 (this issue) covers SCAN_BACKING_OP through SCAN_COMPLETE states; part 1 is W3-89.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sitesurvey cmd handler (backop/complete)
