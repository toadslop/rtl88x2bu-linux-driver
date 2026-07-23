---
title: "[W2-09] Translate ccmp.c (part 1/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-09
epic: E04
blocked_by: [W2-08, T2]
estimate_loc: 200
---

## Goal

Port the first function group (~200 LOC) of [`core/crypto/ccmp.c`](../../../core/crypto/ccmp.c) (~385 total).

## Split-file mechanism (required)

Same as W2-07: move symbols to Rust, extract/remove them from C into `ccmp_rest.c` (or equivalent), Makefile links Rust + rest C — **never** both full `ccmp.c` and Rust exporting the same symbols.

## Acceptance

- No duplicate symbols at link time
- L0 + L1 + L2 green; L4 only at Wave 2 milestone
