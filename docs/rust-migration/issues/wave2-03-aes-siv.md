---
title: "[W2-03] Translate core/crypto/aes-siv.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-03
epic: E04
blocked_by: [W2-02]
estimate_loc: 230
---

## Goal

Exact-translate [`core/crypto/aes-siv.c`](../../../core/crypto/aes-siv.c) (~207 LOC) to Rust; swap Makefile object.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
