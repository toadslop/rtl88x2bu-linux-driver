---
title: "[W4-18] Translate hal_phy_rest.c — RF shadow read/write/refresh"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-18
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port RF register shadow storage helpers from [`hal/hal_phy_rest.c`](../../../hal/hal_phy_rest.c)
(`CONFIG_RF_SHADOW_RW`; built TU when `CONFIG_RUST` — see root `Makefile` `_HAL_PHY_OBJ`) into
[`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- `PHY_RFShadowRead`
- `PHY_RFShadowWrite`
- `PHY_RFShadowRefresh`
- the `static RF_Shadow[...]` table backing those helpers

## Notes

- With `CONFIG_RUST`, the module links `hal/hal_phy_rest.o`, not `hal_phy.c`. The live
  `RF_Shadow` table and all `PHY_RFShadow*` bodies in `88x2bu.ko` today are in
  `hal_phy_rest.c` behind `CONFIG_RF_SHADOW_RW` (same split as [W4-09](wave4-09-hal-phy-bit-shift.md)).
- No `rtw_hal_read_rfreg` in this slice — `PHY_RFShadowCompare` / `PHY_RFShadowRecorver`
  (and `*All` wrappers) stay in `hal_phy_rest.c` for now, but they **read and write the same
  `RF_Shadow` entries** as Read/Write/Refresh. When the table moves to Rust, remaining C shadow
  users must link against **one** Rust-owned store (exported accessors or thin C shim) until a
  later issue ports Compare/Recover with the table.
- Parallel **hal_phy** lane — disjoint from `rust/hal_com.rs` (W4-14…17).
- Guard with the same `CONFIG_RF_SHADOW_RW` cfg as C.

## Acceptance

- `hal_phy_rest.c` retains every symbol **not** moved in this slice (including Compare/Recover
  MMIO paths until explicitly ported)
- L0 build links `rust/hal_phy.o` + `hal_phy_rest.o` into `88x2bu.ko`
- L0 + L1 for Read/Write/Refresh and Rust-exported shadow storage used by remaining C in
  `hal_phy_rest.c` when `CONFIG_RF_SHADOW_RW` is enabled
- L2: host tests for read/write round-trip and refresh zeroing shadow entries
