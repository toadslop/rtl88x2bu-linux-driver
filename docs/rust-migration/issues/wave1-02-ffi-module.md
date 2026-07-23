---
title: "[W1-02] Add rust/ffi module documenting C vs Rust ownership"
labels: [rust-migration, phase-1, wave-1, size/~200]
type: child
id: W1-02
epic: E03
blocked_by: [W1-01]
estimate_loc: 150
---

## Goal

Thin `rust/ffi` (or module tree) that re-exports bindings and documents which symbols/files Rust owns vs C owns during the mixed migration.

## Acceptance

- Clear ownership comments / module layout
- Used (or ready to be used) by the pilot translation
- No behavior change beyond organization

## Out of scope

- Idiomatic safe wrappers (Phase 2)
