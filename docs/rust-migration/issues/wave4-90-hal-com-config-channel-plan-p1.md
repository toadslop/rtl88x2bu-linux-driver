---
title: "[W4-90] Translate hal_com.c — hal_com_config_channel_plan (part 1/2)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-90
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port the **HW-side** branch of `hal_com_config_channel_plan` from [`hal/hal_com.c`](../../../hal/hal_com.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- Entry through `chk_hw_country_code` — `hw_alpha2` lookup via `rtw_get_chplan_from_country`
- `EEPROM_CHANNEL_PLAN_BY_HW_MASK` / `CONFIG_FORCE_SW_CHANNEL_PLAN` handling
- `rtw_is_channel_plan_valid(hw_chplan)` and `force_hw_chplan` early paths
- Leave `chk_sw_config` / `done` in C until part 2 ([W4-91](wave4-91-hal-com-config-channel-plan-p2.md))

## Notes

- **Parallel lane:** `rust/hal_com.rs` — independent of hal_intf in-flight PRs.
- Reuse existing chplan host vectors ([T4](test-04-host-chplan-harness.md) / `rtw_chplan` Rust helpers).
- C stub calls Rust for the HW branch and falls through to remaining C for SW merge.

## Acceptance

- L0 + L1 for the HW-branch helper
- L2: differential cases for hw_alpha2 hit, valid hw_chplan, and force-hw invalid fallback

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)