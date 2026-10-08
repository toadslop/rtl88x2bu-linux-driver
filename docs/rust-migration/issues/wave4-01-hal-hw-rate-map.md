---
title: "[W4-01] Translate hal_com.c — hw_rate_to_m_rate + rate map dump"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-01
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port rate-mapping helpers from [`hal/hal_com.c`](../../../hal/hal_com.c) to
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `hw_rate_to_m_rate`
- `dump_hw_rate_map_test`

## Notes

- First Wave 4 slice: table-driven logic with minimal MMIO — suitable L2 host oracle.
- Introduce `rust/hal_com.rs` module and Kbuild wiring following existing `rtw_*` patterns.
- HAL register dump paths remain in C.

## Acceptance

- L0 build links `rust/hal_com.o` into `88x2bu.ko`
- L1 symbols for ported functions
- L2: host test compares Rust vs C rate map for enumerated `hw_rate` inputs
