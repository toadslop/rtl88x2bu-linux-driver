---
title: "[W3-210] Translate rtw_debug_rest.c — default/driver PHY cap dump leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-210
epic: E05
blocked_by: [W3-209]
estimate_loc: 200
---

## Goal

Port PHY capability debug dump helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_phy_cap.rs`](../../../rust/rtw_debug_phy_cap.rs):

- `rtw_dump_dft_phy_cap`
- `rtw_get_dft_phy_cap`
- `rtw_dump_drv_phy_cap`

## Notes

- `rtw_get_dft_phy_cap` calls `rtw_ht_use_default_setting` / `rtw_vht_use_default_setting` via existing C FFI; no new HAL paths.
- Output is `RTW_PRINT_SEL` formatting only — host L2 compares banner lines against a C oracle with fixture `htpriv` / `registrypriv` fields.
- `CONFIG_BEAMFORMING` sections are string formatting over registry/mlme flags, not beamforming cmd paths.

## Acceptance

- L0 with `CONFIG_80211N_HT` (+ VHT/BF Kconfig as in pinned profile)
- L1 on all three symbols
- L2: golden dump strings for a fixed adapter fixture (subset of cap lines)
