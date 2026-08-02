---
name: pick-up-work-item
description: >-
  Orchestrates the next rust-migration agent action. Auto-applies on "pick up
  work", "find open work item", "find driver work", "check GitHub issues", "what should we work on
  next", "triage issues and start work", or similar. Chooses exactly ONE of
  three paths per run: (A) prepare eligible open/draft PRs when at least one
  still needs prep; (B) otherwise triage issues, select ready work, plan ~200-line
  implement, open PRs ready for review, and babysit; (C) otherwise draft new
  issues per allowlist when backlog is not saturated and chain head is not
  in-flight, then stop. If Path A runs but no PR could be changed (e.g.
  landed on a PR branch already merge-ready), fall through to B/C without
  stopping. Do NOT use for reviewing PRs
  (pr-review-delivery) or for preparing a single named PR in isolation
  (prepare-pr-for-merge).
metadata:
  subskills:
    - prepare-all-prs-for-merge
    - triage-open-issues
    - select-ready-issue
    - plan-stacked-prs
    - implement-stacked-prs
    - draft-migration-issues
  requires-skill: babysit
---

# Pick Up Work Item

Use this skill when the user wants you to **do the next right thing** for the
rust-migration — prepare existing PRs, start new implementation, or refill the
issue backlog.

**Choose exactly one path per invocation.** Do not chain paths in a single run
(e.g. do not prepare PRs and then implement a new issue in the same session),
**except** when Path A makes **no PR changes** — then fall through to Path B or C
(issue triage/selection/drafting) without stopping (see **Landed on a PR branch
but no PRs could be changed**).

This repo tracks work on **GitHub Issues** with draft specs in
[`docs/rust-migration/issues/`](../../../docs/rust-migration/issues/README.md).
Each implementable child targets **~200 lines** of meaningful change
(**≤250 changed lines per PR** — see [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md)).

## PR size (Path B — mandatory)

Oversized PRs (400–600 lines) are a recurring failure mode. Path B **must**:

1. **Plan:** every PR row in the stack table has an estimated Δ ≤ 250
2. **Implement:** run `git diff --shortstat` against the stack base **before each
   commit**; do not open a PR above 250 changed lines
3. **Split:** when scope grows, add PRs to the stack — never ship one large PR

Details: [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#pr-size-limit-mandatory--read-first)
and [`implement-stacked-prs`](../implement-stacked-prs/SKILL.md#3-pr-size-gate-mandatory--before-commit).

## PR descriptions — tag @toadslop (mandatory)

Every pull request opened or materially updated during this workflow must
**tag `@toadslop`** in the description so the maintainer is notified for review.

| When | Requirement |
|------|-------------|
| `ManagePullRequest` `create_pr` | Include `@toadslop` in `body` (first paragraph or a `**Reviewer:** @toadslop` line) |
| `gh pr create` | Same — `@toadslop` in `--body` |
| `ManagePullRequest` `update_pr` | If the existing body lacks `@toadslop`, add it in the same update |
| Knit follow-up PRs (Path A via `prepare-pr-for-merge`) | Same rule |

This applies to Path B implementation PRs, Path C docs PRs, and any follow-up
PRs opened while preparing existing work. Do not omit the tag on stacked PRs
(PR2+).

## Path selection (mandatory — run first)

Before triage, planning, or implementation, list open PRs and apply the same
**eligibility filter** as [`prepare-all-prs-for-merge`](../prepare-all-prs-for-merge/SKILL.md#phase-1--discover-and-filter-prs) Phase 1:

```bash
gh pr list --state open --json number,title,isDraft,baseRefName,headRefName,url
```

For each PR, include it in **`eligible`** only if `baseRefName` is `master`, or
that base branch is already merged into `master` (see prepare-all-prs Phase 1).
All other open PRs go in **`skipped`** (stacked on an unmerged parent).

### Needs-prep vs merge-ready (mandatory — do not stop early)

**Eligible PRs alone do not trigger Path A.** After building `eligible`, classify
each PR as **`needs_prep`** or **`merge_ready`** (nothing for the agent to do
right now). Only PRs in **`needs_prep`** send you to Path A.

For each PR in `eligible`, inspect:

```bash
gh pr view <number> --json isDraft,baseRefName,mergeable,mergeStateStatus,reviewDecision,statusCheckRollup,reviews
```

| Classify **`needs_prep`** when any of | Classify **`merge_ready`** when all of |
|---------------------------------------|----------------------------------------|
| `isDraft` is `true` | `isDraft` is `false` |
| `baseRefName` ≠ `master` (parent merged — retarget/rebase still required) | `baseRefName` is `master` |
| `mergeable` is `CONFLICTING`, or `mergeStateStatus` is `BEHIND` / `DIRTY` / `BLOCKED` | `mergeable` is `MERGEABLE` and branch is current with `master` |
| Required checks failing or still pending | All required checks **success** |
| `reviewDecision` is `CHANGES_REQUESTED`, or a review is **in progress** | No in-progress review; no blocking requested-changes feedback |
| Open blocking review threads the author must address | Approved or awaiting maintainer merge only (optional nits do **not** block Path B/C) |

**Do not stop** just because open PRs exist. When every eligible PR is
**`merge_ready`**, treat the PR queue as handled and **fall through** to Path B or
C — triage issues, select ready work, or draft a new wave. Note merge-ready PRs in
the final report (links + "awaiting merge / maintainer").

### Landed on a PR branch but no PRs could be changed (mandatory fall-through)

Cloud runs often **check out a registered PR branch** at start. That landing does
**not** mean Path A must end the run. After path selection (or after a Path A prep
attempt), if **no PR could be changed** — nothing to retarget/rebase, no draft to
open, no CI/review fixes to push, and **no eligible PR remains `needs_prep`** —
**do not stop**. Treat PR prep as a no-op and **continue immediately** to Path B or C:

**Fall-through preconditions (mandatory):** only when **no eligible PR remains
`needs_prep`** after classification or after the prep batch. If any eligible PR
is still `needs_prep` because prepare hit a **blocker** (requested-changes the
agent cannot address, ambiguous stack, exhausted flaky CI, user input needed, etc.),
**do not fall through** — end with **`human action required`** per **Path A blocked**
below.

**Triggers for fall-through** (no remaining `needs_prep` among eligible PRs):

- Every eligible PR is `skipped` or `merge_ready`
- Path A ran but made zero commits (nothing to fix on merge-ready PRs)
- Landed on a PR branch that is already merge-ready

1. [`triage-open-issues`](../triage-open-issues/SKILL.md)
2. [`select-ready-issue`](../select-ready-issue/SKILL.md) or
   [`draft-migration-issues`](../draft-migration-issues/SKILL.md)

Announce in chat, e.g. "On PR branch `#N` / `cursor/…`; no PRs needed changes —
falling through to issue triage." This overrides the usual Path A **stop here**
rule **only** when the prep batch was a true no-op (no `needs_prep` PRs remain).

| Condition | Path | Action |
|-----------|------|--------|
| **One or more `needs_prep` PRs** among `eligible` | **A — Prepare PRs** | Run [`prepare-all-prs-for-merge`](../prepare-all-prs-for-merge/SKILL.md) on those PRs only; **stop** if any PR was changed or any `needs_prep` PR remains blocked (**human action required**). **Fall through** to B/C only when prep completes with zero changes and no eligible PR is still `needs_prep` (see **Landed on a PR branch but no PRs could be changed**) |
| **No `needs_prep` PRs** (no open PRs, every open PR is `skipped`, or all `eligible` PRs are `merge_ready`) and a ready issue exists after triage + selection | **B — New work** | Triage → select → plan → implement → open PRs → babysit, then **stop** |
| **No `needs_prep` PRs** and selection reports **chain head in-flight (this issue only)** | **B — stack downstream** or **A** | If a **downstream** issue is ready (dep satisfied via open PR), run Path B and stack on the dependency PR branch. If only the chain-head issue itself is in-flight, prep/babysit (Path A) when `needs_prep`; otherwise report merge-ready PRs — **not** Path C |
| **No `needs_prep` PRs** and selection reports **chain head blocked (no accessible code)** or **backlog saturated** | **Stop** or **C** | Blocked head: report blocker; saturated backlog: report count — do **not** draft more tickets unless true tranche gap |
| **No `needs_prep` PRs** and no ready issue, backlog **not** saturated, true tranche gap (chain head has **no accessible code**) | **C — Draft wave** | Triage → draft new issues per **`draft-migration-issues` allowlist**, then **stop** |

When open PRs exist but **`eligible` is empty** (all `skipped`), **fall through**
to Path B or C — do not enter Path A with nothing to prepare. When `eligible` is
non-empty but **`needs_prep` is empty**, **also fall through** — merge-ready PRs
are not a reason to idle. Note skipped PRs, blocking parents, and merge-ready PRs
in the final report.

```mermaid
flowchart TD
  A[Start: pick up work] --> B[List open PRs + eligibility filter]
  B --> C{Any eligible PRs?}
  C -->|Yes| C2{Any needs_prep?}
  C2 -->|Yes| D[Path A: prepare-all-prs-for-merge]
  D --> D2{Prep outcome?}
  D2 -->|Changed at least one PR| Z[Report and stop]
  D2 -->|Blocked — needs_prep remain| Z2[Human action required — stop]
  D2 -->|True no-op — fall through| E
  C2 -->|No — all merge_ready| E[1. Triage open issues]
  C -->|No| E
  E --> F[2. Select ready issue]
  F -->|Found| G[3. Plan stacked PRs]
  G --> H[4. Implement + open PRs + babysit]
  H --> Z
  F -->|None ready| I{Backlog saturated or blocked head?}
  I -->|Yes| Z3[Report frontier — stop]
  I -->|No — true gap| J[Path C: draft per allowlist]
  J --> Z
```

Announce which path you chose in chat (one line), e.g. "Path A — 2 eligible
open PRs need prep; running prepare-all-prs-for-merge", "Path B — 2 open PRs
merge-ready (no prep needed); selecting next issue", or "Path B — 1 open PR
skipped (unmerged parent); no eligible PRs."

## Path A — Prepare all PRs

**When:** path selection found at least one **eligible** PR classified as
**`needs_prep`** (see **Needs-prep vs merge-ready** above). Do **not** enter Path
A when every eligible PR is already **`merge_ready`** — continue to Path B or C
instead.

Load and follow [`prepare-all-prs-for-merge`](../prepare-all-prs-for-merge/SKILL.md)
in full:

1. Filter to PRs whose base is `master` or whose base branch is merged into
   `master` (should match path-selection `eligible` list).
2. Mark eligible drafts ready for review (`gh pr ready`).
3. Run [`prepare-pr-for-merge`](../prepare-pr-for-merge/SKILL.md) on each
   **`needs_prep`** PR (babysit until CI green and reviews complete).

**Stop here** when Path A **changed** at least one PR, **or** any eligible PR
remains `needs_prep` after prep (report **`human action required`**). Fall through
to Path B or C **only** when no eligible PR is still `needs_prep` and the batch
made no changes — per **Landed on a PR branch but no PRs could be changed** above.

Do not triage issues, implement new work, or draft tickets in the same run **when
Path A made real prep progress** or **blocked `needs_prep` PRs remain** — finish
reporting that prep first, then stop.

### Path A blocked — human action required

Path A intentionally prioritizes PR prep over new work. When one or more
**eligible** PRs cannot reach "ready to merge" after prepare (unresolved
requested-changes review, flaky CI exhausted, ambiguous stack, needs a human
decision, etc.):

1. Report each blocker clearly in the final status table (`Ready?` = no, with
   notes).
2. End the run with status **`human action required`** — do not imply the next
   pick-up will automatically unblock the PR.
3. The **next** pick-up will re-enter Path A while those eligible PRs remain
   open (by design).

**User override:** if the user explicitly asks to skip PR prep and start new
work (e.g. "pick up work, ignore open PRs", "start Path B"), honor that and
run Path B or C instead. Note in the report which open PRs were deprioritized.

## Path B — Implement the next ready issue

**When:** no **`needs_prep`** PRs (no open PRs, every open PR is `skipped`, or
every `eligible` PR is **`merge_ready`**), and
[`select-ready-issue`](../select-ready-issue/SKILL.md) finds an unblocked issue
after triage. Skipped and merge-ready open PRs may still exist — note them in the
report.

| Step | Subskill | Outcome |
|------|----------|---------|
| 1 | [`triage-open-issues`](../triage-open-issues/SKILL.md) | Close issues already done but still open |
| 2 | [`select-ready-issue`](../select-ready-issue/SKILL.md) | Pick one open, unblocked, ready issue |
| 3 | [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md) | Split into stacked PRs with **≤250 changed lines each** (target ~200); plan only |
| 4 | [`implement-stacked-prs`](../implement-stacked-prs/SKILL.md) | Implement **every** planned PR in the stack (or file follow-up issues for any remainder); **run size gate before each commit**; open PRs ready for review (not draft); babysit |

### Stack completion (Path B — mandatory)

Autonomous pick-up has **no operator** to answer "should I continue?". During step 4
the agent **must** either:

1. **Complete the full stack** — every row in the plan table becomes an open PR
   with green babysit, or
2. **File follow-up GitHub issue(s)** for every unimplemented plan row before
   stopping, with parent issue comment + blocker documented

**Never** stop after opening some PRs (e.g. 2 of 3) and ask whether to continue,
or end with an implied "next: implement PR3" and no filed tracker. See
[`implement-stacked-prs`](../implement-stacked-prs/SKILL.md#9-continue-the-stack-mandatory--no-partial-stops).

### Open state + babysit (Path B only)

New PRs from this path must land in **open** (ready-for-review) state — not
draft:

- `ManagePullRequest` `create_pr` with `draft: false` (default); **`body` must
  include `@toadslop`** (see **PR descriptions** above).
- `gh pr create` without `--draft`; include `@toadslop` in `--body`.
- If a PR was opened as draft by mistake: `gh pr ready <number>`.

**Babysit each PR** until CI is green before opening the next stack PR (see
[`implement-stacked-prs`](../implement-stacked-prs/SKILL.md) step 7):

1. Load Cursor's built-in **`babysit`** skill when available; otherwise apply its
   intent manually (fix CI, address blocking review feedback, push, re-poll).
2. Poll `gh pr checks` on the PR you just opened until required checks pass.
3. Do **not** start Path A (`prepare-all-prs-for-merge`) in the same run — the
   PRs you just opened will be handled on the **next** pick-up when Path A
   triggers.

**Stop here** after the stack is **fully** implemented and babysitted
(`stack complete`), or after a blocker forced **`stack partial — tracked`**
(follow-up issue(s) filed for every remaining plan row). Do not stop mid-stack
without filing trackers. Do not draft new issues in the same run (except the
mandatory follow-up issues for unfinished stack rows).

## Path C — Draft a new wave

**When:** no **`needs_prep`** PRs (skipped or merge-ready open PRs may remain),
selection finds **no** ready issue, **`draft-migration-issues` When NOT to draft**
checks pass (not in-flight head, backlog not saturated, scope is allowlisted).

| Step | Subskill | Outcome |
|------|----------|---------|
| 1 | [`triage-open-issues`](../triage-open-issues/SKILL.md) | Close stale issues (same as Path B) |
| 2 | [`draft-migration-issues`](../draft-migration-issues/SKILL.md) | Draft only implementable slices with local specs, then **stop** |

**Do not enter Path C** when:

- The chain head issue is unblocked but has **its own** open implementation PR (prep or stack downstream instead)
- ≥15 open children already exist behind the same `blocked_by` chain
- The only "gap" is Wave 4+ HAL/USB/PHYDM scope

**Open PRs on chain-head dependencies do not block Path B or Path C downstream
selection** — dependents can stack on accessible dependency code (see
[`select-ready-issue`](../select-ready-issue/SKILL.md#what-counts-as-blocked-mandatory--read-first)).

Path C is a **complete job** — not a prelude to implementation.

| After Path C | Do | Do not |
|--------------|-----|--------|
| Issues drafted and/or filed | Report what was created, what it unblocks, and any human decisions needed | Re-run selection to implement a new issue |
| User asked only to draft/file issues | Commit/push draft markdown and `ISSUE-MAP.md` updates; open a docs PR if appropriate | Continue to `plan-stacked-prs` or `implement-stacked-prs` |
| Newly filed issues look ready | Note them for a **future** pick-up (Path B) | Start planning or coding in the same session |

Do **not** treat "nothing was ready, so I filed new tickets" as permission to
implement one immediately. Wait for an explicit follow-up or a new pick-up run
(Path A will apply once PRs exist).

## Boundaries

| Do | Do not |
|----|--------|
| Choose **one** path per run and complete it | Chain Path A + B, or B + C, in one session (except empty Path A → fall through to B/C) |
| Prefer Path A when **eligible** PRs **`need_prep`** | Start new implementation while eligible PRs need prep |
| Fall through to B/C when open PRs are all `skipped` or all `merge_ready` | Enter Path A with zero `needs_prep` PRs |
| Fall through to B/C when on a PR branch but no PRs could be changed | Stop after an empty Path A with no issue triage |
| Continue to triage/select/draft when PRs are merge-ready | Stop the run only because open PRs exist |
| Report `human action required` when eligible PRs stay blocked | Imply the next pick-up will fix blocked PRs automatically |
| Honor explicit user override to skip PR prep | Ignore a clear "start new work" instruction |
| Open new PRs ready for review (Path B) | Open implementation PRs as drafts |
| Tag `@toadslop` in every PR description | Omit maintainer notification on new/updated PRs |
| Babysit new PRs until CI is green (Path B) | Skip babysit after opening a stack |
| Complete the full planned stack (Path B) | Stop mid-stack and ask whether to continue |
| File follow-up issue(s) when the stack cannot finish (Path B) | End with "next: implement PRn" and no tracker |
| Draft new issues only when allowlist + gap checks pass (Path C) | Draft deep single-lane chains across unrelated C files |
| Favor wide parallel issue graphs when drafting (Path C) | Chain every new ticket to the previous ID by default |
| Stack new work on open dependency PR branches (Path B) | Wait for chain-head PRs to merge before implementing dependents |
| Stop when backlog saturated — implement/merge instead | File HAL/USB/PHYDM slices without Wave 4 infra |
| Close issues with evidence they are done | Merge PRs without explicit user instruction |
| Query GitHub for open PRs, issues, blocked state | Rewrite `ISSUE-MAP.md` by hand (use `file-issues.sh`) |

## Repo context (quick reference)

- **Tracker (authoritative):** GitHub Issues — `gh issue list` / `gh issue view`
- **Filing registry:** [`ISSUE-MAP.md`](../../../docs/rust-migration/issues/ISSUE-MAP.md) — draft ID ↔ GitHub `#N` (not status)
- **Draft specs:** `wave*`, `test-*`, `arch-*`, `release-*` — templates for filing (~200 LOC)
- **Epics:** `epic-*.md` — wave/phase planning structure (`E01`–`E12`)
- **Labels:** `rust-migration`, `wave-*`, `phase-*`, `size/~200`
- **Gates:** L0 build, L1 symbols, L2 host tests, L3 QEMU, L4 hardware — see [`test-plan.md`](../../../docs/rust-migration/test-plan.md) and [`AGENTS.md`](../../../AGENTS.md)

## Final status report

After completing **one** path, reply in chat with:

| Item | Value |
|------|-------|
| **Path chosen** | A (prepare PRs) / A (no-op) → B / A (no-op) → C / B (implement) / C (draft wave) |
| Open PRs at start | total / eligible / needs_prep / merge_ready / skipped — or "none" |
| Triage | Issues closed (`#N` + reason) or "none" / "n/a (Path A with changes)" / triage results after A no-op fall-through |
| Selected issue | Draft ID, GitHub `#N`, title, **stack base** — Path B only |
| Plan | PR stack table with **Est. Δ ≤ 250 per row** — Path B only |
| Implementation | PR links, babysit/CI status, **`stack complete`** or **`stack partial — tracked`** + follow-up issue links — Path B only |
| PR prep | Per-PR status from prepare-all — Path A only |
| New drafts | Files/issues created — Path C only |
| Workflow end | `prepared PRs` / `human action required` / `A no-op → B/C` / `stack complete` / `stack partial — tracked` / `stopped after drafting` |

Ask the user before starting Path B implementation if they only wanted triage or
selection.

If Path C ran, **do not** ask whether to implement a new issue — the answer is
no unless the user sends a new, explicit instruction.

## Relationship to other skills

| Skill | When |
|-------|------|
| **`prepare-all-prs-for-merge`** | Path A — at least one eligible PR that `needs_prep` |
| **`prepare-pr-for-merge`** | Invoked inside Path A per eligible PR |
| **`plan-stacked-prs`** | Path B — before writing code |
| **`implement-stacked-prs`** | Path B — build, open (non-draft), babysit |
| **`draft-migration-issues`** | Path C — no ready work and no eligible PRs |
| **`pr-review-delivery`** | Reviewer role only — not part of this workflow |
