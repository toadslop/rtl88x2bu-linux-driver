---
name: select-ready-issue
description: >-
  Path B step 2 of pick-up-work-item. Selects one open rust-migration issue that
  is unblocked, not in-flight elsewhere, and ready to implement. Auto-applies
  after triage-open-issues when no PRs need prep (no open PRs, all skipped, or all
  merge-ready). Do NOT use to arbitrarily
  skip dependency order without documenting why.
metadata:
  parent-skill: pick-up-work-item
  path: B
  step: 2
---

# Select Ready Issue

Pick **one** open issue to work on next. Prefer the **earliest unblocked child**
in the current active wave.

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
| Issues with open PRs in flight | check `gh pr list --state open` and issue Notes (`In-flight: …`) |
| Issues whose `blocked_by` deps are **open** | parse **only** the `Blocked by:` line (see below) |
| Issues already closed on GitHub | triage miss — skip |

### Parse `blocked_by` correctly (mandatory)

The `## Tracking` footer has **two** link lines. Only one is a dependency gate:

| Line | Blocks work? |
|------|----------------|
| `- **Epic:** #68 (E05)` | **No** — parent for rollup only; epic may stay open while children land |
| `- **Blocked by:** #255 (W3-38)` | **Yes** — every `#N` on this line must be **CLOSED** |

**Wrong:** treating every `#N` in the Tracking section as `blocked_by` (this
mis-counts the epic and makes every child look blocked).

**Right:** read only the line starting with `- **Blocked by:**` (or YAML
`blocked_by` in a local draft spec). Map draft IDs via
[`ISSUE-MAP.md`](../../../docs/rust-migration/issues/ISSUE-MAP.md) when needed:

```bash
gh issue view <number> --json body,state
# Tracking footer example:
# - **Epic:** #68 (`E05`)          ← NOT a blocker
# - **Blocked by:** #255 (`W3-38`) ← gate; #255 must be CLOSED
```

### Find the chain head (frontier diagnosis)

When selection returns nothing, report **why** using this order:

1. **Chain head in-flight** — lowest-ID open child whose `blocked_by` deps are
   all closed, but an open PR references that draft ID (e.g. W3-39 with PR stack
   open). This is **not** an empty frontier — hand off to Path A prep on those
   PRs (see `pick-up-work-item`), **not** Path C drafting.
2. **Chain head blocked** — lowest-ID open child still waiting on an open
   `blocked_by` issue (name the blocker `#N` / draft ID).
3. **Backlog already filed** — many open children exist behind the head (e.g.
   W3-40…W3-129 all blocked by one in-flight slice). Path C should **not** file
   more issues in the same chain.
4. **Wave complete** — active wave children are closed; future-wave epics only.

## 3. Readiness rules

An issue is **ready** when:

1. **Dependencies closed** — every issue on the `Blocked by:` line is `CLOSED`
   (epic parent open is OK)
2. **No conflicting in-flight work** — no open PR for the same draft ID
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

1. **Unblock others** — issues on the critical path for the current wave
2. **Same wave, lowest ID** — e.g. `W3-04` before `W3-07`
3. **Test infra** (`T*`) when it unblocks CI for in-flight translation work
4. **Architecture** (`A*`) when a wave child lists it in `blocked_by`
5. **User override** — if the user named an issue, use that

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
| **Ready issue found** (including oversized) | Report selection; continue to **`plan-stacked-prs`** |
| **Chain head in-flight** | Report frontier issue + open PR links; parent **`pick-up-work-item`** should prep/babysit those PRs (Path A) — **do not** draft new issues |
| **Nothing ready — true gap** | Open children missing for the next tranche **and** no in-flight chain head **and** fewer than **≥15 open children** behind the same chain head — hand off to Path C **`draft-migration-issues`** |
| **Nothing ready — backlog saturated** | **≥15 open children** already filed behind the same chain head — report chain head + count; **stop** — do not draft more tickets |
| **Ambiguous** | List top 2–3 candidates with tradeoffs; ask user if they care |

## Selection report template

```markdown
**Selected:** W3-04 / #115 — [W3-04] Translate rtw_security.c part 1 — type string helpers

**Why:** W3-03 closed; no open PR; blocked_by W3-03 satisfied; next in Wave 3 sequence.

**Spec:** docs/rust-migration/issues/wave3-04-security-type-str.md

**Epic:** E05 (#68)

**Gates:** L0 + L1 + L2 (T5 harness)
```
