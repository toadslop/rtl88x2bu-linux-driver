---
title: "[W1-04] Pilot Makefile object swap + smoke verification"
labels: [rust-migration, phase-1, wave-1, size/~200]
type: child
id: W1-04
epic: E03
blocked_by: [W1-03]
estimate_loc: 80
---

## Goal

Finish the pilot loop: Makefile points at the Rust object instead of `aes-ctr.o`, document the swap pattern for later issues, run full smoke checklist.

## Note

May be folded into W1-03 if that PR is still under ~200 LOC total.

## Acceptance

- `aes-ctr.c` no longer in the default object list
- `docs/rust-migration.md` documents the per-file swap recipe
- Smoke checklist green
