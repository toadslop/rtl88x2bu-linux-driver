---
title: "[T10] CI: refactor workflow path filters for branch-protection compatibility"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T10
epic: E10
blocked_by: [T3, T6, T7]
estimate_loc: 120
---

## Goal

Make PR status checks **report Success when out of scope** so repo admins can enable branch protection ([T9](test-09-merge-gates.md)) without blocking docs-only or narrowly scoped PRs.

## Background

`Module L0 build`, `Module L1 symbols`, and `Host L2 tests` use **workflow-level** `paths:` filters. When a PR does not touch matching paths, the workflow never runs and required checks stay **Waiting for status to be reported** — merge is blocked even though no driver code changed. This is documented in [`contributing.md`](../../contributing.md) and [`dev-environment.md`](../dev-environment.md#branch-protection) but not fixed in workflows.

## Proposed approach

1. Remove workflow-level `paths:` from the three PR gates (or keep `paths:` only on `push` to `master` where appropriate).
2. Add job-level scope detection, e.g. [`dorny/paths-filter`](https://github.com/dorny/paths-filter) or equivalent `if:` expressions mirroring today's path lists.
3. When out of scope, run a no-op job step that exits 0 and reports the check name (or use `paths-filter` `skip` pattern with a trivial success job).
4. Update [`contributing.md`](../../contributing.md) and [`dev-environment.md`](../dev-environment.md#branch-protection) to remove the "admin bypass for docs-only PRs" workaround once landed.

## Acceptance

- Docs-only PRs against `master` show all three required checks as **Success** (skipped or no-op), not pending forever
- Driver/translation PRs still run the full L0/L1/L2 jobs when relevant paths change
- Check names remain `Host L2 tests / host-l2`, `Module L0 build / module-l0`, `Module L1 symbols / module-l1` (or document any rename for admins)
- [`test-plan.md`](../test-plan.md) T9 CI status note updated if needed

## Out of scope

- Enabling branch protection on GitHub (repo admin; see T9)
- Changing L3 post-merge-only policy (see T15)
