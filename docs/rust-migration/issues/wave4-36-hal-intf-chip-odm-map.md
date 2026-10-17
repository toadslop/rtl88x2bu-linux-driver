---
title: "[W4-36] Translate hal_intf.c — chip_type_to_odm_ic_type lookup"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-36
epic: E06
blocked_by: [W4-07]
estimate_loc: 120
---

## Goal

Move the `_chip_type_to_odm_ic_type[]` table and lookup from [`hal/hal_intf.c`](../../../hal/hal_intf.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_chip_type_to_odm_ic_type(chip_type: u8) -> u32` (or equivalent) with identical indices/values for RTL8822B and neighbors
- C calls Rust instead of duplicating the table in `hal_intf_rest.c`

## Notes

- Extends [W4-07](wave4-07-hal-intf-dump-trx-mode.md) `rust/hal_intf.rs` — same object swap lane, no HAL MMIO.
- Table is read-only data; good L2 oracle (vector of chip enum → ODM IC type).

## Acceptance

- L0 links `rust/hal_intf.o` with lookup symbol
- L1 symbol present; C references Rust for ODM IC type resolution
- L2: golden table spot-check (including RTL8822B / RTL8822C rows)
