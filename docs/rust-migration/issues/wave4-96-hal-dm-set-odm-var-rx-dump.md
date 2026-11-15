---
title: "[W4-96] Translate hal_dm.c — SetHalODMVar RX info dump"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-96
epic: E06
blocked_by: [W4-92, W4-93, W4-94]
estimate_loc: 120
---

## Goal

Port `HAL_ODM_RX_INFO_DUMP` and `HAL_ODM_RX_DFRAME_INFO` cases of `SetHalODMVar`
from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs).

Uses Rust helpers from [W4-92](wave4-92-hal-dm-phydm-igi-iqk-queries.md) and
[W4-93](wave4-93-hal-dm-min-rssi-phy-cnt-queries.md) for IGI, RSSI, and FA counters.
Keeps optional `DBG_RX_*` dump calls on C FFI when Kconfig enables them.

## Notes

- Chain after [W4-94](wave4-94-hal-dm-set-odm-var-core.md) in the same module (merge `switch` in one Rust fn).
- `HDATA_RATE` and multi-RSSI formatting may call existing HAL string FFI.

## Acceptance

- L0 + L1
- L2: capture `_RTW_PRINT_SEL` output for linked/unlinked adapter fixtures vs C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-92, W4-93, W4-94