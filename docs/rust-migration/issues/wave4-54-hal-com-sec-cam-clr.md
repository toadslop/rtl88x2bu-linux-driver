---
title: "[W4-54] Translate hal_com_rest.c — security CAM clear + GK probe"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-54
epic: E06
blocked_by: [W4-52]
estimate_loc: 60
---

## Goal

Port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_sec_clr_cam_ent` — zero first CAM slot via `rtw_sec_write_cam`
- `rtw_sec_read_cam_is_gk` — read ctrl word, test `BIT6`

## Notes

- Depends on Rust `rtw_sec_read_cam` / `rtw_sec_write_cam` from [W4-52](wave4-52-hal-com-sec-cam-rw.md).
- Disjoint hal_com lane from W3 debug proc issues in the same batch.

## Acceptance

- L0 on pinned profile
- L1 symbols for both functions
- GK bit interpretation unchanged
