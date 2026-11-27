---
title: "[W4-155] Translate hal_com_rest.c — LPS frame constructors (PSPoll/beacon/null)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-155
epic: E06
blocked_by: []
estimate_loc: 180
---

## Goal

Port WOW/LPS frame builders from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_lps_frame.rs`:

- `rtw_hal_construct_PSPoll` (static → exported via C shim as today)
- `rtw_hal_construct_beacon`
- `rtw_hal_construct_NullFunctionData`

## Notes

- **Parallel lane:** 802.11 header assembly for PS-Poll, minimal beacon template, and
  null/QoS-null data — no full IE walk in beacon (`#if 0` body stays trimmed as in C).
- `rtw_hal_construct_NullFunctionData` infra/AP/IBSS address field layout must match.
- Used by WOW/LPS reserved-page setup later; independent of in-flight W4-143…145 H2C lane.

## Acceptance

- L0 + L1
- L2: golden frame lengths + frame control/subtype/first addr fields for infra null and PSPoll

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)