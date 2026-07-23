---
title: "[T1] Add check-symbols.sh for C→Rust object ABI gate (L1)"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T1
epic: E10
blocked_by: [T0]
estimate_loc: 200
---

## Goal

Script + docs to compare global symbols between old `.o` and new Rust `.o` when swapping a translation unit.

## Deliverables

- `docs/rust-migration/scripts/check-symbols.sh`
- Make convenience target or documented invocation
- Example usage in test-plan.md

## Acceptance

- Fails if a previously global symbol disappears
- Documented allowlist mechanism for intentional renames (should be rare in Phase 1)
