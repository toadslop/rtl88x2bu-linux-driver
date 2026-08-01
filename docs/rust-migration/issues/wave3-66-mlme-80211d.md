---
title: "[W3-66] Translate rtw_mlme.c — 802.11d country IE processing"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-66
epic: E05
blocked_by: [W3-65]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `process_80211d`

## Notes

- Country IE / regulatory processing from beacon/probe response.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for 802.11d country IE processing
