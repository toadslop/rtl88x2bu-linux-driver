---
title: "[W4-109] Translate hal_dm.c — HALMAC DM leaf delegates"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-109
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Port HALMAC-backed helpers from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_rfe_ctrl_gpio`
- `rtw_phydm_fw_iqk`

## Notes

- **Parallel lane:** thin delegates to `rtw_halmac_*`; no register-format parsing here ([W4-110](wave4-110-hal-dm-cfg-phy-para.md)).
- Stubs return failure when `RTW_HALMAC` is off, matching C.

## Acceptance

- L0 + L1 with default HALMAC-enabled 8822B profile
- L2: mock dvobj records `rtw_halmac_rfe_ctrl_cfg` / `rtw_halmac_iqk` arguments

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)