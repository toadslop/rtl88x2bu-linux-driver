---
title: "[W4-133] Translate hal_com_rest.c — MBSSID MAC address set and change"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-133
epic: E06
blocked_by: [W4-126]
estimate_loc: 160
---

## Goal

Under `CONFIG_MBSSID_CAM`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com_mbid_cam.rs`](../../../rust/hal_com_mbid_cam.rs):

- `rtw_hal_set_macaddr_mbid`
- `rtw_hal_change_macaddr_mbid`

## Notes

- **MBSSID lane (part 4):** calls `write_mbssid_cam` from [W4-126](wave4-126-hal-com-mbid-cam-hw-push.md) and `rtw_mbid_camid_alloc` / `rtw_mbid_cam_info_change` from W4-119/W4-120.
- `change` rejects NULL MAC with `rtw_warn_on(1)`; `set` logs assigned entry id on success.
- TODO block under `#if 0` stays in C — do not port dead code.

## Acceptance

- L0/L1 with `CONFIG_IFACE_NUMBER=3` and concurrent MBSSID profile (same as W4-119)
- L2: host fixture seeds CAM alloc/change → verify `write_mbssid_cam` shim args vs C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** [W4-126](wave4-126-hal-com-mbid-cam-hw-push.md) (`write_mbssid_cam` Rust body)