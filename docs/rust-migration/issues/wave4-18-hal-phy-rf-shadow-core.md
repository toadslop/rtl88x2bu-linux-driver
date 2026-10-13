---
title: "[W4-18] Translate hal_phy.c — RF shadow read/write/refresh"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-18
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port RF register shadow storage helpers from [`hal/hal_phy.c`](../../../hal/hal_phy.c)
(`CONFIG_RF_SHADOW_RW`) into [`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- `PHY_RFShadowRead`
- `PHY_RFShadowWrite`
- `PHY_RFShadowRefresh`

## Notes

- Shadow array only — no `rtw_hal_read_rfreg` in this slice (compare/recover deferred).
- Parallel **hal_phy** lane — disjoint from `rust/hal_com.rs` (W4-14…17).
- Guard with the same `CONFIG_RF_SHADOW_RW` cfg as C.

## Acceptance

- L0 + L1 for the three symbols when `CONFIG_RF_SHADOW_RW` is enabled in the build
- L2: host tests for read/write round-trip and refresh zeroing shadow entries
