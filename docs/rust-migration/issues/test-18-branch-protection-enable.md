---
title: "[T18] Admin: enable branch protection with L0/L1/L2 required checks"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T18
epic: E10
blocked_by: []
estimate_loc: 20
---

## Priority

**P1 (small, high leverage)** — Repo admin task only (~15 minutes). Do this once; do **not** spin up agent work ahead of open translation slices. Workflows and docs already exist ([T9](test-09-merge-gates.md)); this closes the gap where merges may still bypass CI.

## Goal

Turn on **branch protection** on `master` so the three merge gates in [`contributing.md`](../../contributing.md) actually block bad merges.

## Background

- [T9](test-09-merge-gates.md) landed the PR template, contributing docs, and check names.
- [T10](test-10-ci-path-filter-refactor.md) ensures out-of-scope PRs report **Success** on L0/L1/L2 (no forever-pending checks).
- Until Settings are updated, contributors can merge without green CI.

## Deliverables

1. GitHub → **Settings** → **Branches** → rule for `master`:
   - Require a pull request before merging (direct pushes optional per team preference).
   - Require status checks to pass (minimum):
     - `Host L2 tests / host-l2`
     - `Module L0 build / module-l0`
     - `Module L1 symbols / module-l1`
2. Verify with a throwaway PR or docs-only PR: required checks appear and out-of-scope jobs skip cleanly (T10 no-op pattern — **do not** add checks that only run on a workflow-level `paths:` filter; docs-only PRs would stay pending forever).
3. Rewrite [`dev-environment.md`](../dev-environment.md#branch-protection): replace the **“Do not require `module-l3` on pull requests”** row with the enabled rule — date, which checks are required, and that **L3 stays advisory on PRs** until [T19](test-19-branch-protection-extended-checks.md) lands its L3 workflow prerequisite.
4. If [`contributing.md`](../../contributing.md) still lists L3 as post-merge only, add a one-line cross-link to T19 for PR-scoped L3 (no code change required here beyond docs).

## Acceptance

- `master` merges require the three L0/L1/L2 checks above (screenshot or internal confirmation in issue comment is fine).
- [`contributing.md`](../../contributing.md) still matches the enabled check names (L0/L1/L2 only for this issue).
- A `docs/**`-only PR merges without **Waiting for status to be reported** on any required check.

## Out of scope

- Adding new workflows (see [T19](test-19-branch-protection-extended-checks.md), [T20](test-20-ci-clippy-warn.md)).
- Requiring L3 on every PR ([T15](test-15-ci-l3-pr-scope.md) scope is intentional).

## Notes

- No in-repo code required unless check names drifted from workflow job ids.
- Translation agents should **not** pick this up unless they are also repo admins.
