---
title: "[W4-09] Translate hal_phy.c — PHY_CalculateBitShift"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-09
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Introduce [`rust/hal_phy.rs`](../../../rust/hal_phy.rs) and port the pure bitmask helper
from [`hal/hal_phy.c`](../../../hal/hal_phy.c):

- `PHY_CalculateBitShift`

## Notes

- Leaf PHY helper with no MMIO — suitable host L2 oracle (enumerate `BitMask` inputs).
- `hal/hal_phy.c` also contains RF-shadow paths behind `CONFIG_RF_SHADOW_RW`; leave
  them in C (`hal/hal_phy_rest.c`) when swapping to `rust/hal_phy.o`.
- Independent lane: new `rust/hal_phy.rs` module (disjoint from `hal_com` / `hal_intf`).

## Acceptance

- `hal_phy_rest.c` holds every `hal_phy.c` symbol **not** moved to Rust in this slice
- L0 build links `rust/hal_phy.o` + `hal_phy_rest.o` into `88x2bu.ko`
- L1 symbol for `PHY_CalculateBitShift`
- L2: host test compares Rust vs C for representative masks (0, single-bit, multi-bit low)
