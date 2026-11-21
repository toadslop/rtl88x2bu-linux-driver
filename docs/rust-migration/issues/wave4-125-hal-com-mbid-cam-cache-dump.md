---
title: "[W4-125] Translate hal_com_rest.c — MBSSID CAM cache dump"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-125
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Under `CONFIG_MBSSID_CAM`, port `rtw_mbid_cam_cache_dump` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com_mbid_cam.rs`](../../../rust/hal_com_mbid_cam.rs) (W4-119/W4-120 module).

## Notes

- **MBSSID lane (part 3):** formatter only — walks `mbid_cam_ctl` under `lock` and prints bitmap/cache rows; does not MMIO.
- Defer `rtw_mbid_cam_dump` + `read_mbssid_cam` (HW CAM poll) to a follow-up slice to keep this host-L2 friendly.
- Reuse MLME role strings (`STA`/`AP`/`MESH`/`NONE`) matching C `MLME_IS_*` gates.

## Acceptance

- L0/L1 with `CONFIG_IFACE_NUMBER=3` and `CONFIG_CONCURRENT_MODE` (same gate as W4-119)
- L2: extend `tests/host/hal/test_hal_com_mbid_cam.c` — seeded bitmap/cache → dump line oracle vs C

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none — W4-119/W4-120 on `master`)