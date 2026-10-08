---
title: "[W4-03] Translate hal_com.c — dump_chip_info formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-03
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `dump_chip_info` from [`hal/hal_com.c`](../../../hal/hal_com.c) to
[`rust/hal_com.rs`](../../../rust/hal_com.rs).

## Notes

- Formats `HAL_VERSION` / chip ID strings for debug proc paths — no register reads.
- Keep `HAL_VERSION` FFI type from existing bindgen headers.

## Acceptance

- L0 + L1
- Optional L2 golden strings for representative `ChipVersion` enum values
