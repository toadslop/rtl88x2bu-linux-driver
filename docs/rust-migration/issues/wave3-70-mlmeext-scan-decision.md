---
title: "[W3-70] Translate rtw_mlme_ext.c — scan sparse and channel decision"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-70
epic: E05
blocked_by: [W3-69]
estimate_loc: 280
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `rtw_scan_sparse` (uses file-static `token` for sparse division)
- `rtw_scan_backop_decision` (`#ifdef CONFIG_SCAN_BACKOP`)
- `rtw_scan_timeout_decision`
- static helpers: `rtw_scan_ch_decision`, `sitesurvey_res_reset`

## Notes

- Scan planning helpers; `sitesurvey_cmd_hdl` FSM stays in C.
- **Combined ~278 LOC in C (two sub-slices, one issue — see W3-55):**
  - ~115 LOC — sparse/backop/timeout (`rtw_scan_sparse`, `rtw_scan_backop_decision`,
    `rtw_scan_timeout_decision`; note file-static `token` in sparse).
  - ~163 LOC — channel decision + sitesurvey reset (`rtw_scan_ch_decision`,
    `sitesurvey_res_reset`; both `static`, called from sitesurvey path).
- At implement time, split into two PRs if either sub-slice exceeds ~250 changed lines.
- L2: new `tests/host/mlme_ext/` harness with JSON differential vectors for sparse
  channel lists, backop flags, and timeout math.

## Acceptance

- L0 build + L2 host unit tests for scan sparse/channel decision helpers
