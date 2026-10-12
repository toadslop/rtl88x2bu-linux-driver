---
title: "[W4-10] Translate hal_halmac.c — HALMAC network/beacon ctrl mappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-10
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Introduce [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) and port HALMAC↔driver
enum/struct field mappers from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_network_type_halmac2drv`
- `_beacon_ctrl_halmac2drv`
- `_beacon_ctrl_drv2halmac`

## Notes

- Table/bitfield translation only — no SDIO/USB register I/O in this slice.
- Extract unported `hal_halmac.c` symbols to `hal/hal_halmac_rest.c` before swapping
  to `rust/hal_halmac.o` (same `*_rest.c` pattern as `hal_com`).
- Expose `#[no_mangle]` FFI wrappers (or test-only shims) so L2 can call the mappers
  without a live `halmac_adapter`.
- Independent lane: new `rust/hal_halmac.rs` (disjoint from `hal_phy` / `hal_com`).

## Acceptance

- L0 build links `rust/hal_halmac.o` + `hal_halmac_rest.o`
- L1 symbols for the three mappers (or documented test entry points used in L1)
- L2: host vectors for every `halmac_network_type_select` value and representative
  `halmac_bcn_ctrl` / `rtw_halmac_bcn_ctrl` bit patterns
