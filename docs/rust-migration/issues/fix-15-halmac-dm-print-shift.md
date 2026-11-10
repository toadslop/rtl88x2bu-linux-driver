---
title: "[FX-15] hal_halmac/hal_dm: shift panic, dropped mapper warnings, RTW: RTW: prefix"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-15
epic: E14
blocked_by: []
estimate_loc: 70
---

## Priority

**P3 (low)** — latent panic plus log parity.

## Problem

1. **Shift panic** — [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) ~494–499
   (1590cbc3 W4-24 PR2 #1311): `_phy_parameter_val_drv2halmac` does
   `val << bitshift(msk)` after `if msk_en == 0 { return val }`. With `msk_en=1,
   msk=0`, `bitshift` returns 32 → `panic_const_shl_overflow` (present in
   `rust/hal_halmac.o`). In-tree callers set `msk_en = mask ? 1 : 0`, so latent.
2. **Dropped warnings** — rsvd-page mapper (538e38f9 #1245) and RF-type mappers
   (f9e26058 #1299) no longer print `RTW_WARN("%s: Fail to allocate RSVD page(%d)!!")`
   / `RTW_ERR("%s: Invalid RF type(0x%x)!")` in their default arms.
3. **Double prefix** — [`rust/hal_dm_dump_sta_traffic.inc.rs`](../../../rust/hal_dm_dump_sta_traffic.inc.rs)
   (~269, ~278, e29fb3e8 W4-32 #1332) builds `"RTW: [TP] …"` then emits via
   `RTW_PRINT_SEL`, which adds `RTW: ` again → `RTW: RTW: [TP]` through `RTW_DBGDUMP`.
   (The Smooth TP total now sums smooth TX + smooth RX, fixing a C bug — note as spec change.)

## Fix

`if msk_en == 0 || msk == 0 { return val }`; add small C log shims for the mapper
defaults; drop `RTW: ` from the two labels (or emit through `_RTW_PRINT_SEL`).

## Acceptance

- L0; L1 unchanged.
- `llvm-objdump -dr rust/hal_halmac.o`: no `panic_const_shl_overflow` in
  `_phy_parameter_val_drv2halmac`.
- `strings 88x2bu.ko` contains the two restored warnings.
