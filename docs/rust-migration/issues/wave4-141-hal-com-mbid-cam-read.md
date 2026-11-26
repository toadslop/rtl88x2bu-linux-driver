---
title: "[W4-141] Translate hal_com_rest.c — MBSSID CAM read and dump"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-141
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Port MBSSID CAM read path from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_mbid_cam_read.rs` under `CONFIG_MBSSID_CAM`:

- static `read_mbssid_cam`
- `rtw_mbid_cam_dump`

## Notes

- **Parallel lane:** new Rust module — does not extend `hal_com_mbid_cam.rs` (W4-119…126).
- Poll loop on `REG_MBIDCAMCFG_2` must match C timeout behavior (`poll % 10`).
- **`write_mbssid_cam` is already Rust** (`rust/hal_com_mbid_cam.rs`, W4-126). `hal_com_rest.c`
  only forward-declares it; this read/dump slice must **not** redefine or duplicate that symbol.
  Dump is read-only and does not call the write path.

## Acceptance

- L0 + L1 with `CONFIG_MBSSID_CAM=y` on pinned 8822B profile
- L2: stub MMIO read sequence for seeded CAM index vs C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)