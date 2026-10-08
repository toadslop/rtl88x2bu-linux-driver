---
title: "[W3-133] Translate rtw_debug.c — security CAM dump helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-133
epic: E05
blocked_by: [W3-130]
estimate_loc: 200
---

## Goal

Port security CAM debug formatters from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs):

- `dump_sec_cam_ent`, `dump_sec_cam_ent_title`
- `dump_sec_cam`, `dump_sec_cam_cache`

## Notes

- Uses `struct sec_cam_ent` and adapter CAM tables — FFI borrows at entry; no HAL MMIO.
- Align field formatting with existing security Rust modules for MAC/key display.

## Acceptance

- L0 + L1 symbol parity
- Optional L2: format fixed `sec_cam_ent` fixtures to golden strings
