---
title: "[T3] CI: run host L2 tests on every PR"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T3
epic: E10
blocked_by: [T2]
estimate_loc: 150
---

## Goal

GitHub Actions (or equivalent) runs `tests/host` on pull requests. Optionally document/block on L0 when a Rust-enabled kernel container is available later.

## Acceptance

- PR CI runs L2 without hardware
- Failure blocks merge for crypto-related paths (path filters OK)
