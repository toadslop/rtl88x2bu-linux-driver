---
title: "[W4-91] Translate hal_com.c — hal_com_config_channel_plan (part 2/2)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-91
epic: E06
blocked_by: [W4-90]
estimate_loc: 200
---

## Goal

Port the **SW-side** and **`done`** tail of `hal_com_config_channel_plan` from [`hal/hal_com.c`](../../../hal/hal_com.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `chk_sw_config` when `force_hw_chplan` is false — `sw_alpha2` and `sw_chplan` resolution
- `done` — default `0x7F` fallback, `rfctl` / `HalData` channel-plan fields update, country ent pointer
- Export full `hal_com_config_channel_plan` from Rust when both parts are linked

## Notes

- Same-file sequence after [W4-90](wave4-90-hal-com-config-channel-plan-p1.md).
- Preserve printk/RTW_PRINT paths for unsupported alpha2/chplan tuples.

## Acceptance

- L0 + L1 for `hal_com_config_channel_plan`
- L2: end-to-end vectors covering SW-only, HW+SW conflict, and default fallback

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-90