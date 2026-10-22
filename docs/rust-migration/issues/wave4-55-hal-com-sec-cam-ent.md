---
title: "[W4-55] Translate hal_com_rest.c — security CAM entry read/write"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-55
epic: E06
blocked_by: [W4-54]
estimate_loc: 180
---

## Goal

Port CAM entry pack/unpack from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_sec_read_cam_ent`
- `rtw_sec_write_cam_ent` (including `CONFIG_RTL8192F` reorder tail)

## Notes

- Loop uses existing Rust MMIO accessors; preserve ctrl/mac/key slice layout and `begin`/`end` optimization.
- Chains hal_com lane after [W4-54](wave4-54-hal-com-sec-cam-clr.md).

## Acceptance

- L0 on pinned profile (8192F guard compiles when disabled)
- L1 on both symbols
- `rtw_sec_write_cam` call sequence matches C for a fixed test vector (host oracle or L2 if added)
