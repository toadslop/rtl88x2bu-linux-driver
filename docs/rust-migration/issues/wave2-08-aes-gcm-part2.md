---
title: "[W2-08] Translate aes-gcm.c (part 2/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-08
epic: E04
blocked_by: [W2-07]
estimate_loc: 200
---

## Goal

Finish exact-translation of [`core/crypto/aes-gcm.c`](../../../core/crypto/aes-gcm.c); remove the C object from the build.

## Acceptance

- No `aes-gcm.c` in default object list; smoke passes
