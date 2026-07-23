---
title: "[W2-07] Translate aes-gcm.c (part 1/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-07
epic: E04
blocked_by: [W2-06]
estimate_loc: 200
---

## Goal

Exact-translate the first ~200 LOC / first function group of [`core/crypto/aes-gcm.c`](../../../core/crypto/aes-gcm.c) (~326 total). Leave the remainder in C until W2-08 (temporary mixed file ownership is OK if symbols stay unique).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness); L4 only at Wave 2 milestone
