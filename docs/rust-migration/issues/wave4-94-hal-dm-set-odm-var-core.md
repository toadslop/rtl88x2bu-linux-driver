---
title: "[W4-94] Translate hal_dm.c — SetHalODMVar core cases"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-94
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port the first `switch` cases of `SetHalODMVar` from [`hal/hal_dm.c`](../../../hal/hal_dm.c)
into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `HAL_ODM_STA_INFO` — `phydm_cmn_sta_info_hook` set/clear
- `HAL_ODM_P2P_STATE`, `HAL_ODM_WIFI_DISPLAY_STATE`
- `HAL_ODM_REGULATION` — adaptivity domain code refresh
- `HAL_ODM_INITIAL_GAIN` — `odm_pause_dig` resume/pause paths

Leave `HAL_ODM_RX_INFO_DUMP`, debug dump, and antenna-diversity cases in C until [W4-96](wave4-96-hal-dm-set-odm-var-rx-dump.md).

## Notes

- **Parallel lane:** same Rust module as other hal_dm slices but disjoint from in-flight W4-88…92 touch sets. Export `SetHalODMVar` from Rust for the cases above; unported switch arms delegate to a C `set_hal_odm_var_rest` FFI until [W4-96](wave4-96-hal-dm-set-odm-var-rx-dump.md) merges the full `switch` into one Rust fn.
- Preserve `RTW_INFO` logging strings for STA set/clean.

## Acceptance

- L0 + L1 for Rust-exported `SetHalODMVar` entry used from `hal_dm_rest.c` for the cases above
- L2: host oracle per case with mock `dm_struct` / `sta_info`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)