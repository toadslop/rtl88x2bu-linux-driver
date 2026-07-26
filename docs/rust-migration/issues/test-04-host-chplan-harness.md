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

Offline L2 harness for Wave 2 channel-plan ports (mirrors T2 crypto harness):

- Host build of C oracle functions from `core/rtw_chplan.c` (or extracted pure subset)
- JSON or Rust test vectors for plan-id getters, country lookup, DFS helpers
- `make -C tests/host/chplan test`

## Relationship to W2-17

**T4 is the canonical test-infra ID; W2-17a is the implementation branch.** Branch `cursor/w2-17a-chplan-harness-3dd4` already adds `tests/host/chplan/` (Makefile, C oracle, JSON vectors, CI wiring). When that lands, mark T4 done — do not file a duplicate harness issue.

## Acceptance

- Vectors run against C oracle and Rust staticlib/objects
- Wired into PR checklist for W2-17+ chplan issues
- Document in [`test-plan.md`](../test-plan.md) L2 section
