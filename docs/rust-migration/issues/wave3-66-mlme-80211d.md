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

Port helper from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `process_80211d` (static; port as module-private fn)

## Notes

- **`CONFIG_80211D` only** — `static void process_80211d(...)` (~250 LOC in C).
  At implement time, split into two PRs if needed (country IE parse vs channel-plan
  merge), same pattern as W3-55 sub-slices.
- Country IE / regulatory processing from beacon/probe response; called from
  `rtw_survey_event_callback`.
- L2: new `tests/host/mlme/` harness with JSON differential vectors for country IE
  inputs and resulting channel-plan snapshots.

## Acceptance

- L0 build + L2 host unit tests for 802.11d country IE processing
