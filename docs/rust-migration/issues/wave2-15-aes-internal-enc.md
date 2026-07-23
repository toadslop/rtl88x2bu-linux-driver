---
title: "[W2-15] Translate aes-internal-enc.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-15
epic: E04
blocked_by: [W2-14, T2]
estimate_loc: 150
---

## Goal

Exact-translate [`core/crypto/aes-internal-enc.c`](../../../core/crypto/aes-internal-enc.c) (~129 LOC).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
