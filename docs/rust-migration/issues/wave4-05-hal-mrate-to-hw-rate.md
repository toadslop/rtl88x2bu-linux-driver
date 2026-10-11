---
title: "[W4-05] Translate hal_com.c — MRateToHwRate lookup table"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-05
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `MRateToHwRate` and the `_MRateToHwRate[]` table from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `_MRateToHwRate` (static table)
- `MRateToHwRate`

## Notes

- Table-only mapping from `enum MGN_RATE` to hardware descriptor rate — no MMIO.
- `rust/hal_com.rs` already calls `MRateToHwRate` via FFI for `hw_rate_to_m_rate`;
  replace the extern with the Rust implementation and drop the C symbol from
  `hal_com_rest.c` for L1.
- Keep `_HDATA_RATE` / string tables in C for a follow-up slice if needed.

## Acceptance

- L0 + L1 symbol check for `MRateToHwRate`
- L2 host test: enumerate representative `MGN_RATE` values and compare Rust vs C
  oracle for `MRateToHwRate`
