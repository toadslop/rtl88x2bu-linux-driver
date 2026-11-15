---
title: "[W4-95] Translate hal_dm.c — GetHalODMVar"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-95
epic: E06
blocked_by: [W4-92]
estimate_loc: 80
---

## Goal

Port `GetHalODMVar` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `HAL_ODM_INITIAL_GAIN` → `rtw_phydm_get_cur_igi`
- `HAL_ODM_ANTDIV_SELECT` when `CONFIG_ANTENNA_DIVERSITY` (read `rx_idle_ant`)

## Notes

- Depends on [W4-92](wave4-92-hal-dm-phydm-igi-iqk-queries.md) for IGI query parity; stack on the W4-92 PR branch when implementing.
- Small surface — pair with Makefile/ffi wiring only; no RX dump cases here.

## Acceptance

- L0 + L1 symbol parity
- L2: oracle returns expected IGI and antdiv idle ant for fixture `dm_struct`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-92