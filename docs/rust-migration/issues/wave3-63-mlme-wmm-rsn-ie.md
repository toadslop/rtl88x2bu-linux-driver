---
title: "[W3-63] Translate rtw_mlme.c — WMM/RSN IE restructure"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-63
epic: E05
blocked_by: [W3-62]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_restruct_wmm_ie`
- `rtw_restruct_sec_ie`
- `rtw_rsn_sync_pmkid`
- `SecIsInPMKIDList`

## Notes

- Mostly pure IE byte transforms; builds on W3-27/28 WPA/RSN getters.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for WMM/RSN IE restructure helpers
