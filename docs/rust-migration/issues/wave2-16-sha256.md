---
title: "[W2-16] Translate core/crypto/sha256.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-16
epic: E04
blocked_by: [W2-15]
estimate_loc: 120
---

## Goal

Exact-translate [`core/crypto/sha256.c`](../../../core/crypto/sha256.c) (~88 LOC) if not already covered by the Wave 1 pilot; otherwise close as duplicate.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
