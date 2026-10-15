---
title: "[W4-20] Translate hal_phy_rest.c — RF shadow compare/recover"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-20
epic: E06
blocked_by: [W4-19]
estimate_loc: 200
---

## Goal

Port RF shadow MMIO compare/recover helpers from [`hal/hal_phy_rest.c`](../../../hal/hal_phy_rest.c)
(`CONFIG_RF_SHADOW_RW`) into [`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- `PHY_RFShadowCompare`
- `PHY_RFShadowRecorver`
- `PHY_RFShadowCompareAll`
- `PHY_RFShadowRecorverAll`
- `rtw_rust_hal_phy_rf_max_reg` (FFI helper used by `*All` loops)

## Notes

- Same-file follow-up to [W4-19](wave4-19-hal-phy-rf-shadow-flags.md); uses the Rust-owned
  `rtw_rust_rf_shadow` store from [W4-18](wave4-18-hal-phy-rf-shadow-core.md).
- Calls `rtw_hal_read_rfreg` / `rtw_hal_write_rfreg` via existing C HAL entry points — no new
  register accessors in this slice.
- **Dependency rationale:** compare/recover read flag fields set by W4-19 and shadow values from W4-18.

## Acceptance

- L0 + L1 for the five symbols above
- `hal_phy_rest.c` contains no remaining `PHY_RFShadow*` bodies when `CONFIG_RF_SHADOW_RW` is enabled
- L2: host differential tests with stubbed `rtw_hal_read/write_rfreg` for compare mismatch and recover paths
