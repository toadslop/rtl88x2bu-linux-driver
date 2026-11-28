---
title: "[W4-152] Translate hal_com_rest.c — beamforming CSI rate cfg"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-152
epic: E06
blocked_by: [W4-151]
estimate_loc: 45
---

## Goal

Under `CONFIG_BEAMFORMING` and `RTW_BEAMFORMING_VERSION_2`, port
`rtw_hal_beamforming_config_csirate` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_bf.rs` (same module as W4-151).

## Notes

- **In-lane follow-up:** calls `rtw_halmac_bf_cfg_csi_rate`, may tweak `REG_RRSR` and
  `bf_info->cur_csi_rpt_rate` when acting as beamformee (`IS_BEAMFORMEE`).
- Requires W4-151 object swap timing so `hal_com_bf.rs` is already in the build.

## Acceptance

- L0 + L1 on 8822B beamforming profile
- L2: stub HALMAC + RRSR write sequence for RSSI/min CSI index change vs no-op branch

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-151