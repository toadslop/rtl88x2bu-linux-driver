---
name: plan-stacked-prs
description: >-
  Step 3a of pick-up-work-item. Splits a selected migration issue into stacked
  PRs of ~200 lines each and produces an implementation plan. Uses Cursor Plan
  mode when available. Auto-applies after select-ready-issue. Do NOT implement
  code in this step — use implement-stacked-prs next.
metadata:
  parent-skill: pick-up-work-item
  step: 3a
  requires-skill: plan
---

# Plan Stacked PRs

Turn one selected issue into a **stack of ~200 LOC pull requests**. Planning
only — no code changes in this step.

## Prerequisite: Plan mode (preferred)

**Use Cursor's Plan mode** for the breakdown when available:

1. Check your environment for Plan mode (mode switcher or `/plan`).
2. If available, switch to **Plan** and produce the PR stack there — focus on
   file-level scope, dependencies between PRs, and verification gates.
3. Confirm in chat: "Used Plan mode for PR breakdown" (or note if unavailable).

If Plan mode is **not** available, produce the same structured plan inline in
chat. Do not skip planning and jump to implementation.

## Sizing rules

From [`test-plan.md`](../../../docs/rust-migration/test-plan.md) and issue README:

- Target **~200 meaningful lines** per PR (roughly 150–250)
- One logical slice per PR — do not mix unrelated units
- If the issue draft is already ~200 LOC (most `wave*` children), it may be **one PR**
- If larger (or you discover more scope in the C source), split into **2+ stacked PRs**

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

| PR | Base branch | Head branch | Scope (~LOC) | Gates |
|----|-------------|-------------|--------------|-------|
| 1 | `master` | `cursor/w3-04a-type-str-abc1` | Freeze L2 vectors (~180) | L2 |
| 2 | `cursor/w3-04a-type-str-abc1` | `cursor/w3-04b-type-str-rust-abc1` | Rust port + Makefile swap (~200) | L0, L1, L2 |

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
