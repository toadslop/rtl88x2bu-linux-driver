---
title: "[W2-14] Translate aes-internal.c (part 4/4)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-14
epic: E04
blocked_by: [W2-13]
estimate_loc: 250
---

## Goal

Finish [`core/crypto/aes-internal.c`](../../../core/crypto/aes-internal.c); remove C object from build.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
