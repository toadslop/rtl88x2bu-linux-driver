---
title: "[W4-21] Translate hal_halmac.c — HW port + network type drv→halmac"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-21
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with driver→HALMAC enum mappers from
[`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_hw_port_drv2halmac`
- `_network_type_drv2halmac`

## Notes

- Complements [W4-10](wave4-10-hal-halmac-enum-mappers.md) (`_network_type_halmac2drv` and beacon ctrl).
- Table/switch translation only — no `halmac_adapter` or USB/MMIO in this slice.
- Expose `#[no_mangle]` test entry points (same pattern as W4-10) for host L2.
- Independent **hal_halmac** lane root — disjoint from `rust/hal_phy.rs` (W4-20).

## Acceptance

- L0 build links `rust/hal_halmac.o` + `hal_halmac_rest.o`
- L1 symbols (or documented L1 test exports) for both mappers
- L2: vectors for every `enum _hw_port` / `HW_PORT*` value and every `_HW_STATE_*` network type
