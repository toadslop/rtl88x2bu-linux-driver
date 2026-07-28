---
name: pick-up-work-item
description: >-
  Orchestrates picking up the next rust-migration work item from GitHub Issues.
  Auto-applies on "pick up work", "find open work item", "check GitHub issues",
  "what should we work on next", "triage issues and start work", or similar.
  Runs triage (close stale issues), selects a ready item, plans ~200-line stacked
  PRs, implements them, or drafts a large wave of new issues (15–25+ tickets
  when the frontier is empty) when nothing is ready — then stops after drafting
  (do not auto-implement newly filed issues). Do NOT use for
  reviewing PRs (pr-review-delivery) or preparing an existing PR for merge
  (prepare-pr-for-merge).
metadata:
  subskills:
    - triage-open-issues
    - select-ready-issue
    - plan-stacked-prs
    - implement-stacked-prs
    - draft-migration-issues
---

# Pick Up Work Item

Use this skill when the user wants you to **find and start the next piece of
migration work** — not to review or land an existing PR.

This repo tracks work on **GitHub Issues** with draft specs in
[`docs/rust-migration/issues/`](../../../docs/rust-migration/issues/README.md).
Each implementable child targets **~200 lines** of meaningful change.

## Workflow overview

Run these steps **in order**. Do not skip ahead.

| Step | Subskill | Outcome |
|------|----------|---------|
| 1 | [`triage-open-issues`](../triage-open-issues/SKILL.md) | Close issues that are already done but still open |
| 2 | [`select-ready-issue`](../select-ready-issue/SKILL.md) | Pick one open, unblocked, ready issue — or report none |
| 3a | [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md) | Split the issue into ~200 LOC stacked PRs (plan only) |
| 3b | [`implement-stacked-prs`](../implement-stacked-prs/SKILL.md) | Implement PRs one by one, open stacked PRs |
| 4 | [`draft-migration-issues`](../draft-migration-issues/SKILL.md) | **Only if step 2 found nothing ready** — draft a **large wave** of new tickets (15–25+ when frontier empty), then **stop** |

```mermaid
flowchart TD
  A[Start: pick up work] --> B[1. Triage open issues]
  B --> C[2. Select ready issue]
  C -->|Found| D[3a. Plan stacked PRs]
  D --> E[3b. Implement stacked PRs]
  C -->|None ready| F[4. Draft new issues]
  E --> G[Report status]
  F --> H[Report status and stop]
```

## Stop after drafting (step 4)

When step 4 runs, the workflow **ends there**. Creating or filing new issues is a
complete job — not a prelude to implementation. Step 4 should produce **enough
backlog for multiple future pick-ups** (see `draft-migration-issues` batch-size
rules); filing only 1–2 tickets when a whole tranche is unfiled is a workflow
miss.

| After step 4 | Do | Do not |
|--------------|-----|--------|
| Issues drafted and/or filed | Report what was created, what it unblocks, and any human decisions needed | Re-run step 2 to select one of the new issues |
| User asked only to draft/file issues | Commit/push draft markdown, README, and `ISSUE-MAP.md` updates; open a docs PR if appropriate | Continue to `plan-stacked-prs` or `implement-stacked-prs` |
| Newly filed issues look ready | Note them in the report for a **future** pick-up | Start planning or coding in the same session unless the user explicitly asks |

Do **not** treat "nothing was ready, so I filed new tickets" as permission to
immediately implement one. Wait for an explicit follow-up (e.g. "pick up W3-10")
or a new pick-up-work-item run in a later session.

## Boundaries

| Do | Do not |
|----|--------|
| Triage, select, plan, implement, or draft issues | Review someone else's PR (`pr-review-delivery`) |
| Close issues with evidence they are done | Merge PRs without explicit user instruction |
| Open stacked PRs for the selected issue | Pick up a second issue while the first stack is in flight (unless user asks) |
| Stop after step 4 (draft/file issues) with a status report | Auto-select or implement a newly drafted issue in the same run |
| Update `docs/rust-migration/issues/README.md` status when closing issues | Rewrite `ISSUE-MAP.md` by hand (use `file-issues.sh`) |

## Repo context (quick reference)

- **Issue map:** [`ISSUE-MAP.md`](../../../docs/rust-migration/issues/ISSUE-MAP.md) — draft ID ↔ GitHub `#N`
- **Status table:** [`README.md`](../../../docs/rust-migration/issues/README.md) — local done/draft/in-progress tracking
- **Epics:** `epic-*.md` — wave/phase parents (`E01`–`E12`)
- **Children:** `wave*`, `test-*`, `arch-*`, `release-*` — sized ~200 LOC slices
- **Labels:** `rust-migration`, `wave-*`, `phase-*`, `size/~200`
- **Gates:** L0 build, L1 symbols, L2 host tests, L3 QEMU, L4 hardware — see [`test-plan.md`](../../../docs/rust-migration/test-plan.md) and [`AGENTS.md`](../../../AGENTS.md)

## Final status report

After completing the applicable steps, reply in chat with:

| Item | Value |
|------|-------|
| Triage | Issues closed (list `#N` + reason) or "none" |
| Selected issue | Draft ID, GitHub `#N`, title — or "none ready" |
| Plan | PR stack table (branch, scope, ~LOC, gates) — if planned |
| Implementation | PR links opened, current stack position — if implemented |
| New drafts | Files created / issues filed — if step 4 ran |
| Workflow end | `implemented` (steps 3a–3b) or `stopped after drafting` (step 4 only) |

Ask the user before starting implementation if they only wanted triage or selection.

If step 4 ran, **do not** ask whether to implement a new issue — the answer is no
unless the user sends a new, explicit instruction.

## Relationship to other skills

| Skill | When |
|-------|------|
| **`plan-stacked-prs`** | Before writing code — produces the PR breakdown |
| **`implement-stacked-prs`** | After plan approval — builds and opens stacked PRs |
| **`prepare-pr-for-merge`** | Later — when a stack PR is ready to land on `master` |
| **`pr-review-delivery`** | Reviewer role only — not part of this workflow |
