---
title: "[W4-120] Translate hal_com_rest.c — MBSSID CAM alloc and clean"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-120
epic: E06
blocked_by: [W4-119]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_com_mbid_cam.rs`](../../../rust/hal_com_mbid_cam.rs) with
`CONFIG_MBSSID_CAM` helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c):

- `rtw_mbid_camid_alloc`, `rtw_mbid_cam_info_change`, `rtw_mbid_cam_assign`
- `rtw_mbid_camid_clean`

## Notes

- **Same lane as W4-119** — shares `hal_com_mbid_cam.rs` and Makefile object swap.
- Entry count / bitmap invariants must match C under concurrent alloc/clean.

## Acceptance

- L0 + L1 for the four functions above
- L2: differential host cases for alloc collision, assign-to-busy-slot, and clean

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-119 (search + reset helpers)