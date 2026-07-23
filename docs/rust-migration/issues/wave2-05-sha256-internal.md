---
title: "[W2-05] Translate core/crypto/sha256-internal.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-05
epic: E04
blocked_by: [W2-04]
estimate_loc: 250
---

## Goal

Exact-translate [`core/crypto/sha256-internal.c`](../../../core/crypto/sha256-internal.c) (~230 LOC). If the Rust diff exceeds ~250 meaningful lines, split into a follow-up issue before merging.

## Acceptance

- C ABI preserved; module builds/loads; smoke passes
