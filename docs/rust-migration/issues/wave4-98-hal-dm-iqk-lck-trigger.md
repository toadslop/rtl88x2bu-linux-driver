---
title: "[W4-98] Translate hal_dm.c — IQK and LCK RFK triggers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-98
epic: E06
blocked_by: []
estimate_loc: 160
---

## Goal

Port RF calibration trigger wrappers from [`hal/hal_dm.c`](../../../hal/hal_dm.c)
into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs) for the **8822B** profile:

- `rtw_phydm_iqk_trigger`
- `rtw_phydm_iqk_trigger_all`
- `rtw_phydm_lck_trigger`

Delegate to existing `halrf_*` FFI; preserve chip `#if` branches needed for `CONFIG_RTL8822B`.

## Notes

- **Parallel lane:** disjoint from SetHalODMVar work ([W4-94](wave4-94-hal-dm-set-odm-var-core.md)).
- Does not port `rtw_hal_phydm_cal_trigger`, manufacturing `iqk_test`, or watchdog paths.

## Acceptance

- L0 + L1 for the three exports under default USB 8822B kconfig
- L2: mock `dm_struct` records which `halrf_*` entry points were invoked

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)