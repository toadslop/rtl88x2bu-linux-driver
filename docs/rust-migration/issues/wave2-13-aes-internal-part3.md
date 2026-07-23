---
title: "[W2-13] Translate aes-internal.c (part 3/4)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-13
epic: E04
blocked_by: [W2-12, T2]
estimate_loc: 200
---

## Goal

Continue exact-translation of [`core/crypto/aes-internal.c`](../../../core/crypto/aes-internal.c) (~200 LOC chunk).

## Split-file mechanism

Continue W2-11 approach: shrink `aes-internal_rest.c` as symbols move to Rust; no duplicate symbols.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
