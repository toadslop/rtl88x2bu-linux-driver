---
title: "[W3-62] Translate rtw_mlme.c — unassociated STA queue helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-62
epic: E05
blocked_by: [W3-61]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_unassoc_sta_init`
- `rtw_unassoc_sta_deinit`
- `rtw_rx_add_unassoc_sta`
- `rtw_add_interested_unassoc_sta`
- `rtw_undo_interested_unassoc_sta`
- `rtw_search_unassoc_sta`

## Notes

- Queue CRUD under mlme lock. Builds on W3-53 network compare helpers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for unassoc STA queue helpers
