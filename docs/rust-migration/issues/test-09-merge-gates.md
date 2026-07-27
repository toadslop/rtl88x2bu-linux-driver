---
title: "[T9] Enforce merge gates: branch protection, PR template, required checks"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T9
epic: E10
blocked_by: [T3, T6]
estimate_loc: 80
---

## Goal

Make CI gates **actually block bad merges**. Today workflows exist but `master` has **no branch protection** and there is no `.github/PULL_REQUEST_TEMPLATE.md`, so contributors can merge without checks and without the verification checklist from [`test-plan.md`](../test-plan.md).

## Deliverables

1. **Branch protection** on `master` (repo admin):
   - Require pull request before merging
   - Require status checks to pass (at minimum: Host L2 tests; add L0/L1/L3 as they land)
   - Dismiss stale reviews optional; require linear history optional (team preference)

2. **PR template** (`.github/PULL_REQUEST_TEMPLATE.md`) — copy the checklist from test-plan:

   - Characterization / vectors
   - L0 build
   - L1 symbols (when C→Rust swap)
   - L2 host tests (when pure chunk)
   - L3 VM load (when init/USB touched)
   - L4 hardware (wave milestones only)
   - Architecture / domain types

3. **Contributing note** in [`README.md`](../../../README.md): link PR template and list required CI checks.

## Acceptance

- PR template renders on new PRs with L0–L4 checklist
- Branch protection documented in [`dev-environment.md`](../dev-environment.md) or a short `docs/contributing.md` pointer
- After T6/T7/T8 land, required checks list updated to match workflow job names

## Notes

- Branch protection itself is a GitHub Settings change; this issue can land the template + docs while an admin enables protection.
- CodeQL runs via GitHub advanced setup; decide whether to require it (currently dynamic workflow, not in-repo).
