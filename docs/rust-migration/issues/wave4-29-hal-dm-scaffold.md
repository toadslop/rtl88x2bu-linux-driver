---
title: "[W4-29] Kbuild scaffold — rust/hal_dm.rs + hal_dm_rest.c split"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-29
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Introduce [`rust/hal_dm.rs`](../../../rust/hal_dm.rs) and wire Kbuild so [`hal/hal_dm.c`](../../../hal/hal_dm.c)
can shrink to [`hal/hal_dm_rest.c`](../../../hal/hal_dm_rest.c):

- Add `rust/hal_dm.o` when `CONFIG_RUST` is enabled
- Move a minimal `rtw_rust_hal_dm_kbuild_probe` no-op into Rust (W4-30 replaces with `Init_ODM_ComInfo` board-type helper)
- Extract unported symbols into `hal_dm_rest.c` per the `hal_com` / `hal_usb` `*_rest.c` pattern

## Notes

- Parallel **hal_dm** lane — disjoint from `rust/hal_halmac.rs`, `rust/hal_usb.rs`, and PHYDM register paths.
- Do not port `Init_ODM_ComInfo`, watchdog, or `odm_*` MMIO in this slice.

## Acceptance

- L0 links `rust/hal_dm.o` + `hal/hal_dm_rest.o`
- L1: exported symbol callable from C init or a documented test entry point
- L3: QEMU insmod/rmmod unchanged
