---
title: "[W3-71] Translate rtw_mlme_ext.c — sitesurvey channel pick"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-71
epic: E05
blocked_by: [W3-70]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `sitesurvey_pick_ch_behavior`

## Notes

- Channel pick logic for sitesurvey; medium/high coupling to chset (W3-54).
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sitesurvey channel pick behavior
