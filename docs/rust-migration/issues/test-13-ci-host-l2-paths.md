---
title: "[T13] CI: close host-l2 path-filter gaps for *_rest.c sources"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T13
epic: E10
blocked_by: [T3]
estimate_loc: 40
---

## Goal

Ensure **L2 host tests** run when C oracle sources for `*_rest.c` units change, even if `rust/**` and `tests/host/**` are untouched.

## Background

[`.github/workflows/host-l2.yml`](../../../.github/workflows/host-l2.yml) lists explicit `core/rtw_*.c` paths. It includes `core/rtw_io_rest.c`, `core/rtw_rf_rest.c`, `core/rtw_swcrypto_rest.c`, and `core/rtw_ieee80211_rest.c`, but omits:

- `core/rtw_chplan_rest.c`
- `core/rtw_security_rest.c`

A C-only oracle or vector fix in those files would not trigger L2 CI.

## Proposed approach

1. Add the two missing paths to `host-l2.yml` `paths:` for both `push` and `pull_request`.
2. Alternatively (preferred long-term): replace the growing explicit list with `core/**` minus documented exclusions, matching L0's `core/**` glob — only if job-level scoping from T10 is in place to avoid running L2 on unrelated core changes.

For this issue, the minimal fix (add two paths) is sufficient.

## Acceptance

- PR that only changes `core/rtw_chplan_rest.c` or `core/rtw_security_rest.c` triggers `Host L2 tests`
- No change to harness commands (`make -C tests/host/chplan test`, `make -C tests/host/security test`, etc.)

## Out of scope

- T10 path-filter refactor (can land independently)
- New L2 harnesses
