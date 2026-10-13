---
title: "[W4-19] Translate hal_phy.c — RF shadow compare/recover flags"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-19
epic: E06
blocked_by: [W4-18]
estimate_loc: 200
---

## Goal

Port RF shadow flag helpers from [`hal/hal_phy.c`](../../../hal/hal_phy.c)
(`CONFIG_RF_SHADOW_RW`) into [`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- `PHY_RFShadowCompareFlagSet`
- `PHY_RFShadowRecorverFlagSet`
- `PHY_RFShadowCompareFlagSetAll`
- `PHY_RFShadowRecorverFlagSetAll`

## Notes

- Same-file follow-up to W4-18; touches the shared `RF_Shadow` table in `hal_phy.rs`.
- `PHY_RFShadowCompare` / `PHY_RFShadowRecorver` (MMIO) stay in C for a later slice.
- **Dependency rationale:** shared static shadow state with W4-18.

## Acceptance

- L0 + L1 for the four flag helpers
- L2: verify per-offset and `*All` flag patterns match C oracle (reg 0x26/0x27 special case)
