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
- Introduce `rust/hal_com.rs` and Kbuild wiring; `hal/hal_com.c` is ~17k lines as one
  `hal/hal_com.o` — extract unported code to `hal/hal_com_rest.c` before swapping to
  `rust/hal_com.o` (same `*_rest.c` pattern as `wave3-104`).
- `W4-02`…`W4-04` add symbols into `rust/hal_com.rs` while remainder stays in
  `hal_com_rest.c` until fully ported.
- HAL register dump paths remain in C for later slices.

## Acceptance

- `hal_com_rest.c` holds every `hal_com.c` symbol **not** moved to Rust in this slice
- L0 build links `rust/hal_com.o` + `hal_com_rest.o` into `88x2bu.ko`
- L1 symbols for ported functions
- L2: host test compares Rust vs C rate map for enumerated `hw_rate` inputs
