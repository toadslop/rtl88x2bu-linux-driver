---
title: "[W4-02] Translate hal_com.c — reserved page cache helpers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-02
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `struct rsvd_page_cache_t` helpers from [`hal/hal_com.c`](../../../hal/hal_com.c) to
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rsvd_page_cache_update_all`
- `rsvd_page_cache_update_data`
- `rsvd_page_cache_free_data`
- `rsvd_page_cache_free`

## Notes

- Independent parallel lane vs W4-01 (same file but no API dependency).
- Memory lifecycle only — no USB/HAL register access in these functions.
- L2: heap-backed cache fixtures on host with alloc hooks or mock `u8` buffers.

## Acceptance

- L0 + L1 on swapped symbols
- L2 differential tests for update/free sequences
