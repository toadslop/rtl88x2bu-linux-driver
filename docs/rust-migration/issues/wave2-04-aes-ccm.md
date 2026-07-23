---
title: "[W2-04] Translate core/crypto/aes-ccm.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-04
epic: E04
blocked_by: [W2-03]
estimate_loc: 230
---

## Goal

Exact-translate [`core/crypto/aes-ccm.c`](../../../core/crypto/aes-ccm.c) (~211 LOC) to Rust; swap Makefile object.

## Acceptance

- C ABI preserved; module builds/loads; encrypted STA smoke passes
