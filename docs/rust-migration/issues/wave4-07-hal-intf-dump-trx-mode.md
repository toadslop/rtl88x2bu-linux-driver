---
title: "[W4-07] Translate hal_intf.c — TRX mode debug dumps"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-07
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Introduce [`rust/hal_intf.rs`](../../../rust/hal_intf.rs) and port TRX-mode debug
formatters from [`hal/hal_intf.c`](../../../hal/hal_intf.c):

- `dump_hal_runtime_trx_mode`
- `dump_hal_trx_mode`

## Notes

- First `hal_intf.c` slice: read-only formatting of `HAL_DATA_TYPE` path/NSS fields.
- Extract unported `hal_intf.c` symbols to `hal/hal_intf_rest.c` before swapping to
  `rust/hal_intf.o` (same `*_rest.c` pattern as `hal_com`).
- `dump_hal_trx_mode` calls `dump_hal_runtime_trx_mode` — port both in one slice.

## Acceptance

- L0 build links `rust/hal_intf.o` + `hal_intf_rest.o`
- L1 symbols for both dump helpers
- L2 golden output for a synthetic `HAL_DATA_TYPE` + registry fixture
