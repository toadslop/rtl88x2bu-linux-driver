---
title: "[W4-119] Translate hal_com_rest.c — MBSSID CAM init and search"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-119
epic: E06
blocked_by: []
estimate_loc: 180
---

## Goal

Under `CONFIG_MBSSID_CAM`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into a new submodule `rust/hal_com_mbid_cam.rs` (included from `hal_com.rs`):

- `rtw_mbid_cam_init`, `rtw_mbid_cam_deinit`, `rtw_mbid_cam_reset`
- `rtw_mbid_cam_search_by_macaddr`, `rtw_mbid_cam_search_by_ifaceid`
- `rtw_get_max_mbid_cam_id`, `rtw_get_mbid_cam_entry_num`

## Notes

- **Parallel lane:** MBSSID CAM cache/control — disjoint submodule file from in-flight W4-117/118 and active `hal_dm` stacks.
- Static helpers `_rtw_mbid_cam_search_*` stay in C or move behind `rtw_rust_*` shims as needed for L2.

## Acceptance

- L0 build with `CONFIG_MBSSID_CAM` on the pinned profile
- L1 symbols for exported search/init helpers above
- L2: host fixture drives bitmap/cache state; search results match C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)