---
title: "[W4-11] Translate hal_halmac.c — reserved page count mappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-11
epic: E06
blocked_by: [W4-10]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with reserved-page count
translation from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c):

- `_rsvd_page_num_drv2halmac`
- `_rsvd_page_num_halmac2drv`

## Notes

- Same-file follow-up to W4-10 (`hal_halmac_rest.c` remainder stays in C).
- **Dependency rationale:** shares `rust/hal_halmac.rs` and Kbuild object swap with W4-10.
- Boundary inputs (>256) should match C warning/ clamp semantics in Notes only — L2
  covers the enumerated bucket table.

## Acceptance

- L0 + L1 on both mappers
- L2: round-trip and boundary vectors for page counts {0, 8, 16, 24, 32, 64, 128, 256, 300}
