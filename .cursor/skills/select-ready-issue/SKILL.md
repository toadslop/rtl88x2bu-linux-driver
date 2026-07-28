---
name: select-ready-issue
description: >-
  Step 2 of pick-up-work-item. Selects one open rust-migration issue that is
  unblocked, not in-flight elsewhere, and ready to implement. Auto-applies after
  triage-open-issues. Do NOT use to arbitrarily skip dependency order without
  documenting why.
metadata:
  parent-skill: pick-up-work-item
  step: 2
---

# Select Ready Issue

Pick **one** open issue to work on next. Prefer the **earliest unblocked child**
in the current active wave.

## 1. Determine the active frontier

Read [`docs/rust-migration/issues/README.md`](../../../docs/rust-migration/issues/README.md)
and open epics to see which wave is in progress:

- **Wave 3 tranche 1** (`E05`, #68): `W3-01`…`W3-09` — typical active frontier
- **Test infra** (`E10`): `T6`–`T9` can run in parallel when unblocked
- **Architecture** (`E11`): `A2`, `A3` when Wave 3 needs domain types

Epics for future waves (`E06`–`E09`) are usually **not** ready until the prior
wave's children are mostly done.

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
| Issues whose `blocked_by` deps are open | read Tracking footer or draft frontmatter |
| Rows marked `done` in README (triage miss) | send back to triage |

Resolve `blocked_by` via [`ISSUE-MAP.md`](../../../docs/rust-migration/issues/ISSUE-MAP.md):

```bash
gh issue view <number> --json body,state
# Tracking footer: Blocked by: #113 (W3-02)
```

## 3. Readiness rules

An issue is **ready** when:

1. **Dependencies closed** — every `blocked_by` issue is `CLOSED`
2. **No conflicting in-flight work** — no open PR for the same draft ID
3. **Spec exists** — matching `docs/rust-migration/issues/<file>.md` with Goal + Acceptance
4. **Sized for ~200 LOC** — if the draft is larger than ~200 LOC, the issue is
   still **ready**; continue to **`plan-stacked-prs`** to split it into a
   multi-PR stack for the same issue. Do **not** route to
   **`draft-migration-issues`** (that step runs only when step 2 finds nothing
   ready).

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
| **Nothing ready** | Explain why (blocked, all in-flight, wave complete); continue to **`draft-migration-issues`** — which must draft a **large wave** (15–25+ issues) when the frontier is empty, not 1–2 tickets |
| **Ambiguous** | List top 2–3 candidates with tradeoffs; ask user if they care |

## Selection report template

```markdown
**Selected:** W3-04 / #115 — [W3-04] Translate rtw_security.c part 1 — type string helpers

**Why:** W3-03 closed; no open PR; blocked_by W3-03 satisfied; next in Wave 3 sequence.

**Spec:** docs/rust-migration/issues/wave3-04-security-type-str.md

**Epic:** E05 (#68)

**Gates:** L0 + L1 + L2 (T5 harness)
```
