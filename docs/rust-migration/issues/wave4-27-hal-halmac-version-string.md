---
title: "[W4-27] Translate hal_halmac.c — rtw_halmac_get_version formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-27
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `rtw_halmac_get_version` from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c) into
[`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs):

- Call `halmac_get_version` via existing FFI
- Format `V{major}_{prototype:02d}_{minor:02d}` into the caller buffer with `rtw_sprintf` semantics
- No-op / empty string on `HALMAC_RET_SUCCESS` failure paths (match C)

## Notes

- Independent hal_halmac slice — disjoint touch set from W4-26 (additive Rust + thin C stub).
- L2 uses a stub `halmac_get_version` returning fixed `halmac_ver` fields.

## Acceptance

- L0 + L1: `rtw_halmac_get_version` symbol exported from Rust
- L2: oracle compares formatted string for representative version tuples and failure status
