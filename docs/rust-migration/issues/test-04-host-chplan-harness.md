---
title: "[T4] Host chplan differential harness + vectors"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T4
epic: E10
blocked_by: [T2]
estimate_loc: 200
---

## Goal

Extend offline test infrastructure for Wave 3 channel-plan ports (mirrors T2 crypto harness):

- Host build of C oracle functions from `core/rtw_chplan.c` (or extracted pure subset)
- JSON or Rust test vectors for plan-id getters, country lookup, DFS helpers
- `make -C tests/host/chplan test` (or subdirectory of `tests/host/`)

## Acceptance

- Vectors run against C oracle and Rust staticlib/objects
- Wired into PR checklist for W3-02+ chplan issues
- Document in [`test-plan.md`](../test-plan.md) L2 section
