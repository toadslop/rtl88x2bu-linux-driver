---
title: "[W2-02] Translate core/crypto/gcmp.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-02
epic: E04
blocked_by: [W2-01]
estimate_loc: 220
---

## Goal

Exact-translate [`core/crypto/gcmp.c`](../../../core/crypto/gcmp.c) (~194 LOC) to Rust; swap Makefile object.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
