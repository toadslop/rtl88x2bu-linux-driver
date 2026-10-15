---
title: "[W4-22] Translate hal_halmac.c — RF type drv↔halmac mappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-22
epic: E06
blocked_by: [W4-21]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with RF type translation from
[`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_rf_type_drv2halmac`
- `_rf_type_halmac2drv`

## Notes

- Same-file follow-up to [W4-21](wave4-21-hal-halmac-drv-network-port.md) in the hal_halmac lane.
- Includes `HALMAC_RF_2T2R_GREEN` → `RF_2T2R` mapping and invalid-type error path semantics.
- **Dependency rationale:** sequential edits to `hal_halmac.rs` / `hal_halmac_rest.c` extraction.

## Acceptance

- L0 + L1 on both mappers
- L2: round-trip for each supported `enum rf_type` / `enum halmac_rf_type` pair plus invalid input
