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
- **Locking contract:** `rtw_mbid_cam_info_change` holds `mbid_cam_ctl->lock` and must call W4-119's lock-free `_rtw_mbid_cam_search_by_ifaceid` (not the public `rtw_mbid_cam_search_by_*`, which re-enter the lock). `rtw_mbid_camid_alloc` / `rtw_mbid_cam_assign` use the locking mac search only **before** they acquire the CAM lock.
- Invalid ids (`>= TOTAL_MBID_CAM_NUM` or `INVALID_CAM_ID` `0xFF`) hit `rtw_warn_on` but still update bitmap/cache (clean path also `ATOMIC_DEC`) — no early return; match C parity.

## Acceptance

- L0/L1 with `CONFIG_IFACE_NUMBER=3` and `CONFIG_CONCURRENT_MODE` (same MBSSID CAM gate as W4-119)
- L2: alloc collision, assign-to-busy-slot, clean, and invalid-id warn-on paths (bitmap/cache still updated)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-119 (search + reset helpers)