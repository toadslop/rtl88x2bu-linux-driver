---
title: "[W3-107] mbo IE parse/build leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-107
epic: E05
blocked_by: [W3-106]
estimate_loc: 180
---

## Goal

Port helpers from [`core/rtw_mbo.c`](../../../core/rtw_mbo.c) to [`rust/rtw_mbo.rs`](../../../rust/rtw_mbo.rs):

- `rtw_mbo_ie_get`
- `rtw_mbo_attrs_get`
- `rtw_mbo_attr_sz_get`
- `rtw_mbo_build_mbo_ie_hdr`
- `rtw_mbo_disallowed_network`
- `rtw_mbo_non_pref_chan_exist`

## Notes

- MBO IE parse/build leaf helpers; low-medium coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mbo IE parse/build leaf helpers
