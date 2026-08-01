---
title: "[W3-72] Translate rtw_mlme_ext.c — band IE update and mgnt format"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-72
epic: E05
blocked_by: [W3-71]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `change_band_update_ie`
- `format`

## Notes

- Mgmt frame IE layout on band change.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for band IE update and mgnt format
