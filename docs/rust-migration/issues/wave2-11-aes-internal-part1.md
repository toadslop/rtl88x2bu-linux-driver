---
title: "[W2-11] Translate aes-internal.c (part 1/4)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-11
epic: E04
blocked_by: [W2-10]
estimate_loc: 200
---

## Goal

Exact-translate first ~200 LOC / function group of [`core/crypto/aes-internal.c`](../../../core/crypto/aes-internal.c) (~843 total). Document which symbols moved in the PR description.

## Acceptance

- No duplicate symbols; smoke passes
