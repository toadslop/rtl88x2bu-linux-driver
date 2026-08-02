---
name: select-ready-issue
description: >-
  Path B step 2 of pick-up-work-item. Selects one open rust-migration issue that
  is unblocked (deps closed or accessible via open PR), not already in-flight
  elsewhere, and ready to implement. Auto-applies after triage-open-issues when
  no PRs need prep (no open PRs, all skipped, or all merge-ready). Do NOT use to
  arbitrarily skip dependency order without documenting why.
metadata:
  parent-skill: pick-up-work-item
  path: B
  step: 2
---

# Select Ready Issue

Pick **one** open issue to work on next. Prefer a **ready** issue in a parallel
lane with no in-flight PR (see §4); when lanes tie, prefer the earliest unblocked
child in the current active wave.

## 1. Determine the active frontier

Query GitHub for the active wave — do not read a local status table:

```bash
gh issue list --label rust-migration --state open --limit 200 \
  --json number,title,labels
```

Read [`docs/rust-migration/issues/README.md`](../../../docs/rust-migration/issues/README.md)
and open `epic-*.md` files for wave structure (not open/closed state):

- **Wave 3** (`E05`, #68): `W3-*` children — typical active frontier
- **Test infra** (`E10`): `T6`–`T9` can run in parallel when unblocked
- **Architecture** (`E11`): `A2`, `A3` when Wave 3 needs domain types

Epics for future waves (`E06`–`E09`) are usually **not** ready until the prior
wave's children are mostly **closed on GitHub**.

## 2. Build the candidate list

```bash
gh issue list --label rust-migration --state open --limit 200 \
  --json number,title,labels
```

Exclude:

| Exclude | Reason |
|---------|--------|
| `[Epic]` issues | tracking parents, not implementable slices |
| Issues with **their own** open PR in flight | check `gh pr list --state open` and issue Notes (`In-flight: …`) — another agent is already implementing this slice |
| Issues whose `blocked_by` deps are **unsatisfied** | parse **only** the `Blocked by:` line; a dep is unsatisfied only when its code is **not accessible** (see below) |
| Issues already closed on GitHub | triage miss — skip |

### What counts as blocked? (mandatory — read first)

A ticket is **blocked** only when a `blocked_by` dependency has **no written,
accessible code** — i.e. the dependency issue is still open **and** there is no
open PR on GitHub that implements it.

A `blocked_by` dependency is **satisfied** (does **not** block dependents) when
**either**:

| Condition | Why it unblocks |
|-----------|-----------------|
| GitHub issue is **CLOSED** | Code merged to `master` |
| An **open PR** implements the dependency | Code exists on a GitHub branch; stack new work on top of that PR |

**Wrong:** treating every open `blocked_by` issue as blocking — this ignores open
PR stacks that already contain the dependency code.

**Wrong:** waiting for a chain-head PR to **merge** before selecting the next
child (e.g. W3-40 while W3-39 has an open PR). Merge is not required; branch
off the PR.

**Right:** W3-39 has open PR `#200` on `cursor/w3-39-…` → W3-40 (`blocked_by:
W3-39`) is **ready**; PR1 of W3-40 stacks on `cursor/w3-39-…`, not `master`.

### Parse `blocked_by` correctly (mandatory)

The `## Tracking` footer has **two** link lines. Only one is a dependency gate:

| Line | Blocks work? |
|------|----------------|
| `- **Epic:** #68 (E05)` | **No** — parent for rollup only; epic may stay open while children land |
| `- **Blocked by:** #255 (W3-38)` | **Yes** — every `#N` on this line must be **satisfied** (closed or accessible via open PR) |

**Wrong:** treating every `#N` in the Tracking section as `blocked_by` (this
mis-counts the epic and makes every child look blocked).

**Wrong:** requiring every `#N` on the `Blocked by:` line to be **CLOSED** when
an open PR already carries that slice.

**Right:** read only the line starting with `- **Blocked by:**` (or YAML
`blocked_by` in a local draft spec). Map draft IDs via
[`ISSUE-MAP.md`](../../../docs/rust-migration/issues/ISSUE-MAP.md) when needed:

```bash
gh issue view <number> --json body,state
# Tracking footer example:
# - **Epic:** #68 (`E05`)          ← NOT a blocker
# - **Blocked by:** #255 (`W3-38`) ← gate; #255 must be satisfied (see below)
```

### Check dependency accessibility (mandatory)

For each `#N` / draft ID on the `Blocked by:` line:

```bash
# 1. Issue state
gh issue view <N> --json state,title

# 2. If OPEN — search for an implementing open PR (draft ID, #N, branch from Notes)
gh pr list --state open --search "W3-38" --json number,title,headRefName,baseRefName,url
gh pr list --state open --search "<N>" --json number,title,headRefName,baseRefName,url
```

| Dep state | Open PR found? | Satisfied? |
|-----------|----------------|------------|
| `CLOSED` | (any) | **Yes** — use `master` (or merged stack) as base |
| `OPEN` | **Yes** — PR title/body/branch references the dep draft ID or `#N` (do not rely on search alone — confirm `headRefName` or Notes `In-flight:` matches) | **Yes** — record `headRefName` as **stack base** for PR1 |
| `OPEN` | **No** | **No** — issue is blocked; stop checking this candidate |

When multiple deps are listed, **all** must be satisfied. For PR1 base, use the
**tip of the dependency chain** — the `headRefName` of the open PR for the
**latest** (highest-ID / last in chain) unsatisfied-by-merge but satisfied-by-PR
dependency. If every dep is closed, base is `master`.

### Find the chain head (frontier diagnosis)

When selection returns nothing, report **why** using this order:

1. **Chain head blocked (no accessible code)** — lowest-ID open child whose
   `blocked_by` deps include at least one issue that is **open with no
   implementing PR** (name the blocker `#N` / draft ID). This is the true
   frontier — code has not been written or is not on GitHub yet.
2. **Chain head in-flight (accessible, not selectable)** — lowest-ID open child
   whose deps are all satisfied but **this issue** already has an open PR.
   Report the PR link; hand off to Path A prep if `needs_prep`. **Downstream**
   children (e.g. W3-40 when W3-39 is in-flight) should already be selectable —
   if they are not, re-check dependency accessibility (step above).
3. **Single-lane backlog saturated** — ≥15 open children already filed behind
   the same **single-lane** chain head. Do **not** file more in that lane; hand
   off to Path C to draft **other parallel lanes** (see
   [`draft-migration-issues`](../draft-migration-issues/SKILL.md)).
4. **Wave complete** — active wave children are closed; future-wave epics only.

## 3. Readiness rules

An issue is **ready** when:

1. **Dependencies satisfied** — every issue on the `Blocked by:` line is
   **CLOSED** or has an **open PR** with accessible implementation code (epic
   parent open is OK). Record the stack-base branch when deps are satisfied via
   PR only.
2. **No conflicting in-flight work on this issue** — no open PR for **this**
   issue's draft ID (another agent may already be implementing it)
3. **Spec exists** — Goal + Acceptance in **either** a local
   `docs/rust-migration/issues/<file>.md` **or** the GitHub issue body (live
   body is authoritative after filing). Prefer local markdown when both exist.
4. **Sized for ~200 LOC** — if the draft or C source is larger than ~200 LOC, the
   issue is still **ready**, but **`plan-stacked-prs` must split it into a
   multi-PR stack** (each PR ≤ 250 changed lines). Do **not** implement the
   whole issue in one PR. Do **not** route to **`draft-migration-issues`**
   (that step runs only when step 2 finds nothing ready).

## 4. Prioritization (default order)

When multiple issues are ready, pick the **first** match:

1. **Parallel lanes** — prefer an issue in a C file / module with **no** other
   open in-flight PR (spread work across `rtw_rf.c`, `rtw_recv.c`, `rtw_mlme.c`,
   etc. instead of stacking only one lane)
2. **Unblock others** — issues on the critical path for the current wave
3. **Same wave, lowest ID** within the chosen lane — e.g. `W3-04` before `W3-07`
   when both are in the same file and ready
4. **Test infra** (`T*`) when it unblocks CI for in-flight translation work
5. **Architecture** (`A*`) when a wave child lists it in `blocked_by`
6. **User override** — if the user named an issue, use that

## 5. Load the full spec

For the selected issue:

1. Map GitHub `#N` → draft ID via `ISSUE-MAP.md`
2. Read the draft markdown (frontmatter + Goal + Acceptance + Notes)
3. Read related epic (`epic: E05` → `epic-05-wave3.md`)
4. Skim the C source named in the Goal to confirm scope

```bash
gh issue view <number> --json number,title,body,labels,state
```

## 6. Hand off or fall back

| Outcome | Next step |
|---------|-----------|
| **Ready issue found** (including oversized) | Report selection + **stack base** (`master` or dependency PR branch); continue to **`plan-stacked-prs`** |
| **Chain head in-flight (this issue only)** | Report frontier issue + open PR links; parent **`pick-up-work-item`** should prep/babysit those PRs (Path A) if `needs_prep` — **do not** treat this as blocking downstream issues |
| **Chain head blocked (no accessible code)** | Report blocker `#N` / draft ID; Path C when a true tranche gap exists in another lane |
| **Nothing ready — true gap** | Open children missing for the next tranche **and** chain head has **no accessible code** **and** fewer than 15 open children behind the same **single-lane** chain head — hand off to Path C **`draft-migration-issues`** |
| **Nothing ready — single-lane saturated** | **≥15 open children** already filed behind the same **single-lane** chain head — report chain head + count; hand off to Path C to draft **other parallel lanes** (do not extend that lane) |
| **Nothing ready — whole-wave saturated** | **Every** active parallel lane has ≥15 open children — report counts; **stop** — implement/merge instead |
| **Ambiguous** | List top 2–3 candidates with tradeoffs; ask user if they care |

## Selection report template

```markdown
**Selected:** W3-40 / #256 — [W3-40] …

**Why:** W3-39 open but PR #200 implements it; blocked_by W3-39 satisfied via accessible code; next in Wave 3 sequence.

**Stack base:** `cursor/w3-39-recv-counters-ea1e` (PR #200) — not `master`

**Spec:** docs/rust-migration/issues/wave3-40-….md

**Epic:** E05 (#68)

**Gates:** L0 + L1 + L2
```
