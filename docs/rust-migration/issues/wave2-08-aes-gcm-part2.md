---
title: "[W2-08] Translate aes-gcm.c (part 2/2)"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-08
epic: E04
blocked_by: [W2-07, T2]
estimate_loc: 200
---

## Goal

Finish porting the remaining C from W2-07 (`aes-gcm_rest.c` or equivalent); remove all C objects for this unit from the build.

## Acceptance

- No `aes-gcm*.c` in the default object list
- L0 + L1 + L2 green; L4 only at Wave 2 milestone
