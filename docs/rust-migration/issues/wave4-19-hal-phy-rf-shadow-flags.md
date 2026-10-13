---
title: "[W4-19] Translate hal_phy_rest.c — RF shadow compare/recover flags"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-19
epic: E06
blocked_by: [W4-18]
estimate_loc: 200
---

## Goal

Port RF shadow flag helpers from [`hal/hal_phy_rest.c`](../../../hal/hal_phy_rest.c)
(`CONFIG_RF_SHADOW_RW`) into [`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- `PHY_RFShadowCompareFlagSet`
- `PHY_RFShadowRecorverFlagSet`
- `PHY_RFShadowCompareFlagSetAll`
- `PHY_RFShadowRecorverFlagSetAll`

## Notes

- Same-file follow-up to W4-18; flag helpers touch the **same** `RF_Shadow` entries as
  Read/Write/Refresh and as the deferred `PHY_RFShadowCompare` / `PHY_RFShadowRecorver`
  paths still in `hal_phy_rest.c` (see [W4-18](wave4-18-hal-phy-rf-shadow-core.md) shared-storage note).
- After W4-18, there is no independent C-only `RF_Shadow` — flag setters and still-C MMIO
  compare/recover must use the Rust-owned backing store from W4-18.
- `PHY_RFShadowCompare` / `PHY_RFShadowRecorver` (MMIO) stay in C for a later slice.
- **Dependency rationale:** shared static shadow state with W4-18.

## Acceptance

- L0 + L1 for the four flag helpers
- Remaining C compare/recover in `hal_phy_rest.c` still compile and use the same shadow store as W4-18
- L2: verify per-offset and `*All` flag patterns match C oracle (reg 0x26/0x27 special case)
