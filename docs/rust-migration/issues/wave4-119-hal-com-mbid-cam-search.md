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
- Port `_rtw_mbid_cam_search_by_macaddr` and `_rtw_mbid_cam_search_by_ifaceid` into this slice (lock-free search while the caller holds `mbid_cam_ctl->lock`). W4-120 `rtw_mbid_cam_info_change` calls the iface helper under that lock; public search wrappers take/drop the lock before delegating. Keeping unlocked search in C deadlocks against non-recursive `_enter_critical_bh`.
- L2 can extend `tests/host/hal/test_hal_com_mbid_cam.c` (C oracle on `master`; wire `RUST_HAL_COM_MBID_CAM_ORACLE` when Rust lands).

## Acceptance

- L0/L1 with `CONFIG_IFACE_NUMBER=3` and `CONFIG_CONCURRENT_MODE` (`CONFIG_MBSSID_CAM` is defined in `drv_conf.h` only when `CONFIG_IFACE_NUMBER > 2`; stock pin has 1–2 ifaces and omits these symbols). Makefile exposes `_config_iface_gt2` for Rust cfg.
- L1 symbols for exported search/init helpers above
- L2: host fixture drives bitmap/cache state; search results match C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)