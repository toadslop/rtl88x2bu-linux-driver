---
title: "[W4-101] Translate hal_dm.c — PHYDM calibration trigger orchestration"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-101
epic: E06
blocked_by: [W4-98]
estimate_loc: 80
---

## Goal

Port `rtw_hal_phydm_cal_trigger` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — PS deny / leave power-save, call
`rtw_phydm_iqk_trigger_all`, restore PS deny.

## Notes

- **Dependency rationale:** calls `rtw_phydm_iqk_trigger_all` ported in [W4-98](wave4-98-hal-dm-iqk-lck-trigger.md).
- Does not port manufacturing `rtw_hal_iqk_test` / `rtw_hal_lck_test` (`CONFIG_DBG_RF_CAL`).

## Acceptance

- L0 + L1 symbol parity
- L2: mock adapter records PS deny + `halrf` trigger call order

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-98 (#2086)