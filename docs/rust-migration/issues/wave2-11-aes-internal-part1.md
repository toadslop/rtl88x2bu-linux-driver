---
title: "[W2-11] Translate aes-internal.c (part 1/4)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-11
epic: E04
blocked_by: [W2-10, T2]
estimate_loc: 200
---

## Goal

Port first ~200 LOC / function group of [`core/crypto/aes-internal.c`](../../../core/crypto/aes-internal.c) (~843 total). Document moved symbols in the PR.

## Split-file mechanism (required)

Same as W2-07: Rust gets the moved symbols; remaining C lives in `aes-internal_rest.c` (updated each part); never duplicate symbols across C and Rust. Parts 2–4 shrink `*_rest.c` until it is deleted in W2-14.

## Acceptance

- No duplicate symbols at link time
- L0 + L1 + L2 green; L4 only at Wave 2 milestone
