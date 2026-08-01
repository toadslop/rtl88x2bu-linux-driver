---
title: "[W3-70] Translate rtw_mlme_ext.c — scan sparse and channel decision"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-70
epic: E05
blocked_by: [W3-69]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `rtw_scan_sparse`
- `rtw_scan_backop_decision`
- `rtw_scan_timeout_decision`
- `rtw_scan_ch_decision`
- `sitesurvey_res_reset`

## Notes

- Scan planning helpers; sitesurvey_cmd_hdl FSM stays in C.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for scan sparse/channel decision helpers
