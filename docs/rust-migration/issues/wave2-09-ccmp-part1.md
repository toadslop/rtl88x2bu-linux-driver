---
title: "[W2-09] Translate ccmp.c (part 1/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-09
epic: E04
blocked_by: [W2-08]
estimate_loc: 200
---

## Goal

Exact-translate first function group (~200 LOC) of [`core/crypto/ccmp.c`](../../../core/crypto/ccmp.c) (~385 total).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
