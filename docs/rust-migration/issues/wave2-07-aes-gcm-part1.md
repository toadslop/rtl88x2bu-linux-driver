---
title: "[W2-07] Translate aes-gcm.c (part 1/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-07
epic: E04
blocked_by: [W2-06, T2]
estimate_loc: 200
---

## Goal

Port the first function group (~200 LOC) of [`core/crypto/aes-gcm.c`](../../../core/crypto/aes-gcm.c) (~326 total) to Rust with characterization tests.

## Split-file mechanism (required)

Do **not** leave the same symbols defined in both C and Rust (link will fail).

Intended approach for part 1:

1. Move the chosen function group into Rust (`extern "C"` names preserved) + L2 vectors.
2. **Remove those functions from the C TU** and keep the remainder as `core/crypto/aes-gcm_rest.c` (or delete them in-place from `aes-gcm.c` and rename/keep one remaining C object).
3. Makefile links: Rust object **and** the remaining C object; stop compiling the original full `aes-gcm.c` if it was split/renamed.
4. W2-08 finishes the rest and drops the remaining C object.

## Acceptance

- No duplicate symbols at link time
- L0 + L1 + L2 green; L4 only at Wave 2 milestone
