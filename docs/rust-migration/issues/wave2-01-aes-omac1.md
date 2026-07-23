---
title: "[W2-01] Translate core/crypto/aes-omac1.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-01
epic: E04
blocked_by: [W1-04, T2, A1]
estimate_loc: 200
---

## Goal

Exact-translate [`core/crypto/aes-omac1.c`](../../../core/crypto/aes-omac1.c) (~172 LOC) to Rust; swap Makefile object.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
