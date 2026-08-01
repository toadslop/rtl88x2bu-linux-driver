---
title: "[W3-65] Translate rtw_mlme.c — scanned network update merge"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-65
epic: E05
blocked_by: [W3-64]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `update_network`
- `update_current_network`

## Notes

- Network struct merge from scan results; medium adapter coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for network update merge helpers
