---
name: plan-stacked-prs
description: >-
  Path B step 3 of pick-up-work-item. Splits a selected migration issue into
  stacked PRs of ~200 lines each (hard max 250 changed lines per PR) and
  produces an implementation plan with per-PR LOC estimates. Uses Cursor Plan
  mode when available. Auto-applies after select-ready-issue. Do NOT implement
  code in this step — use implement-stacked-prs next.
metadata:
  parent-skill: pick-up-work-item
  path: B
  step: 3
  requires-skill: plan
---

# Plan Stacked PRs

Turn one selected issue into a **stack of small, independently mergeable pull
requests**. Planning only — no code changes in this step.

## PR size limit (mandatory — read first)

**This is the most common planning failure.** Agents routinely open 400–600 line
PRs when the repo standard is **~200 lines per PR**. Treat the limit as a **hard
constraint**, not a suggestion.

| Rule | Value |
|------|-------|
| **Target** | ~200 changed lines per PR |
| **Hard maximum** | **250 changed lines** — never plan a PR above this |
| **If over max** | Add another PR to the stack; do not ship one large PR |

**How to measure (same check `implement-stacked-prs` runs before opening a PR):**

```bash
git add -A   # stage untracked files before measuring
# Merge-base → working tree (no ..HEAD): includes staged + unstaged changes
git diff --shortstat $(git merge-base HEAD origin/<base>)
# Example output:  12 files changed, 187 insertions(+), 42 deletions(-)
# Budget = insertions + deletions  →  187 + 42 = 229  (OK, under 250)
```

Count **insertions + deletions** against the PR's stack base (`master` for PR1,
previous PR branch for PR2+). This matches what reviewers see on GitHub.

**Plan-time estimation (before writing code):**

1. Skim the C/Rust files in scope; note function groups and line counts.
2. For **each planned PR**, list files and assign an **estimated Δ** (changed lines).
3. If a single PR's estimate exceeds **250**, split it — use the axes below.
4. If the **whole issue** estimates above 250, the stack has **at least 2 PRs**.
   A `size/~200` label or `estimate_loc: 200` on the issue does **not** mean
   "one PR" — it means one *slice*; oversized source still needs multiple PRs.

**Wrong vs right:**

| Wrong | Right |
|-------|-------|
| One 500-line PR for the whole issue | Stack of 3 PRs (~170 + ~190 + ~140) |
| "Tests + port + Makefile swap" in one PR | PR1 vectors, PR2 Rust port, PR3 Makefile swap |
| "I'll keep it small" with no LOC estimate | Every plan row has an estimated Δ ≤ 250 |
| Treating ~200 as "up to 600 is fine" | >250 is a planning bug — split again |

## Prerequisite: Plan mode (preferred)

**Use Cursor's Plan mode** for the breakdown when available:

1. Check your environment for Plan mode (mode switcher or `/plan`).
2. If available, switch to **Plan** and produce the PR stack there — focus on
   file-level scope, dependencies between PRs, and verification gates.
3. Confirm in chat: "Used Plan mode for PR breakdown" (or note if unavailable).

If Plan mode is **not** available, produce the same structured plan inline in
chat. Do not skip planning and jump to implementation.

## Split axes (use when estimate > 250)

From [`test-plan.md`](../../../docs/rust-migration/test-plan.md) and
[`issues/README.md`](../../../docs/rust-migration/issues/README.md):

- One logical slice per PR — do not mix unrelated units
- Default to **more PRs, smaller diffs** when unsure
- Re-split during implementation if the diff grows past 250 — return here

**Good split axes for this repo:**

| Pattern | Example |
|---------|---------|
| Harness / vectors first | L2 oracle before Rust port |
| Makefile object swap last | keep C + Rust parallel until final PR in slice |
| Part 1 / Part 2 | `aes-gcm-part1`, `aes-gcm-part2` (existing convention) |
| Domain types before port | `A2` helpers, then `W3-*` translation |
| FFI seam vs logic | bindgen/abi in one PR, algorithm in the next |

## Stack design

Each PR in the stack:

1. **Builds on the previous PR's branch** (not `master`) — stacked PRs
2. Has a **narrow title** — `[W3-04 PR1] …`, `[W3-04 PR2] …`
3. Lists **gates** to run before opening (L0, L1, L2, L3 as applicable)
4. Maps to a **branch name** — `cursor/<short-desc>-<suffix>` (match repo convention)

```mermaid
flowchart LR
  M[master] --> PR1[PR1 branch]
  PR1 --> PR2[PR2 based on PR1]
  PR2 --> PR3[PR3 based on PR2]
```

| PR | Base branch | Head branch | Est. Δ (lines) | Gates |
|----|-------------|-------------|----------------|-------|
| 1 | `master` | `cursor/w3-04a-type-str-abc1` | ~180 (must be ≤250) | L2 |
| 2 | `cursor/w3-04a-type-str-abc1` | `cursor/w3-04b-type-str-rust-abc1` | ~200 (must be ≤250) | L0, L1, L2 |

Every row **must** show an estimated Δ. Reject your own plan if any row is blank
or above 250.

## Plan contents (required sections)

### Context

- Issue: draft ID, GitHub `#N`, link to spec file
- Epic and wave
- C files / Rust modules in scope

### PR stack table

The table above — one row per PR.

### Per-PR detail

For each PR:

- **Goal** — one sentence
- **Estimated Δ** — insertions + deletions vs stack base (must be ≤250)
- **Files touched** — explicit paths
- **Out of scope** — what this PR deliberately does not do
- **Verification** — commands (from `AGENTS.md` / `test-plan.md`)
- **Risk** — ABI, FFI, flaky tests

### Dependency notes

- `blocked_by` issues already satisfied
- Whether L2 harness must merge before port PR
- Whether this stack blocks other issues

## Get user confirmation

Present the plan and **pause for approval** unless the user said to auto-implement.

- User approves → continue to **`implement-stacked-prs`**
- User edits → revise plan first
- User defers → stop after plan

## Relationship to `implement-stacked-prs`

This skill ends at an approved plan. The next skill creates branches, writes
code, runs gates, and opens PRs with correct stack bases.
