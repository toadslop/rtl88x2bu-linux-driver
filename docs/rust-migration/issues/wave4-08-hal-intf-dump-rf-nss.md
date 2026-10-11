---
title: "[W4-08] Translate hal_intf.c — RF path and TRX NSS dumps"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-08
epic: E06
blocked_by: [W4-07]
estimate_loc: 200
---

## Goal

Port RF/NSS debug formatters from [`hal/hal_intf.c`](../../../hal/hal_intf.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `_dump_rf_path`
- `_dump_trx_nss`

## Notes

- Same-file follow-up to W4-07 (`hal_intf_rest.c` remainder).
- Uses `rf_type_to_rfpath_str` / chip rftype helpers — call existing C FFI or
  port minimal string helpers if already available in Rust from Wave 2/3.

## Acceptance

- L0 + L1
- L2 golden strings for representative registry + `hal_spec_t` + `HAL_DATA_TYPE`
  fixtures
