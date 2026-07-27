---
name: draft-migration-issues
description: >-
  Step 4 of pick-up-work-item (fallback). When no issue is ready, analyzes
  in-progress epics and drafts new ~200 LOC child issues as markdown specs.
  Can file via file-issues.sh. Auto-applies when select-ready-issue finds
  nothing ready. Do NOT use to duplicate existing open issues.
metadata:
  parent-skill: pick-up-work-item
  step: 4
---

# Draft Migration Issues

When **no open issue is ready**, figure out what tickets are missing to keep the
migration moving, then draft them in the repo's issue format.

## 1. Diagnose the gap

Answer these questions from epics, README, and GitHub:

| Question | Sources |
|----------|---------|
| Which epic/wave is active? | `epic-*.md`, open `[Epic]` issues |
| Which children are done vs open? | README status table, `gh issue list` |
| What is blocked and why? | `blocked_by` chains, open PRs |
| What large units lack child slices? | Wave 4+ epics, oversized C files |
| What test infra is missing? | `E10`, `test-*.md`, CI workflow gaps |

```bash
# Epic progress (sub-issues on GitHub)
gh issue view 68 --json title,body,state
gh issue view 68 --json subIssues,subIssuesSummary 2>/dev/null || true
```

Common gap patterns in this repo:

- **Wave tranche 2 not filed** — README says "Later: Wave 3 tranche 2…"
- **Oversized C TU** — needs splitting like `wave2-07` / `wave2-08` (part 1/2)
- **Missing L2 harness** — translation issue blocked until `T4`/`T5`-style work exists
- **Domain types** — `A2`/`A3` not done but wave children list them in `blocked_by`
- **Deferred scope** — epic Notes say "file separate issues when…"

## 2. Decide what to draft

Each new issue must be:

- **~200 LOC** implementable slice (`estimate_loc: 200`, label `size/~200`)
- **Linked** — `epic: E0N`, `blocked_by: [...]` in YAML frontmatter
- **Verifiable** — explicit L0/L1/L2 (and L3/L4 if relevant) in Acceptance
- **Non-duplicative** — search open issues and `ISSUE-MAP.md` first

**Naming conventions:**

| Type | ID pattern | File pattern |
|------|------------|--------------|
| Wave child | `W3-10`, `W4-01` | `wave3-10-*.md` |
| Test infra | `T10` | `test-10-*.md` |
| Architecture | `A4` | `arch-04-*.md` |
| Release | `R2` | `release-02-*.md` |

Increment IDs beyond the highest in `ISSUE-MAP.md` / README.

## 3. Write draft markdown

Use existing children as templates (e.g. `wave3-04-security-type-str.md`):

```yaml
---
title: "[W3-10] Short title"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-10
epic: E05
blocked_by: [W3-09]
estimate_loc: 200
---

## Goal

Port … from [`core/...`](../../../core/...) to [`rust/...`](../../../rust/...).

## Notes

- Why this slice / split here
- What stays in C until a later issue

## Acceptance

- L0 build + L1 symbols + …
```

For a **new epic child batch**, also update the parent `epic-*.md` Children table.

## 4. File on GitHub (optional)

When drafts are ready to publish:

1. Add new filenames to the `files=(…)` array in
   [`file-issues.sh`](../../docs/rust-migration/issues/file-issues.sh) if not
   already listed
2. Run from repo root:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

The script is idempotent — it skips IDs already in `ISSUE-MAP.md`.

3. Update README status table with new rows (`draft` / `open`)
4. Commit draft markdown + README + `ISSUE-MAP.md` changes

**Ask the user** before filing to GitHub if they only wanted local drafts.

## 5. Sub-issue linking (GitHub)

After filing, link sub-issues to the epic parent when `gh` supports it:

```bash
gh issue edit <child-number> --add-parent <epic-number>
# or during create:
gh issue create --parent <epic-number> ...
```

Sub-issues roll up progress on the parent epic in GitHub Projects.

## 6. Report

```markdown
**Gap:** Wave 3 tranche 1 complete; `rtw_security.c` TKIP still in C; no W3-10+ filed.

**Drafted:**
| ID | File | Title | Blocked by |
|----|------|-------|------------|
| W3-10 | wave3-10-security-tkip-p1.md | TKIP phase1 helpers | W3-09 |

**Filed on GitHub:** yes / no — #121

**Unblocks:** W3-07 follow-on work after W3-10 merges

**Recommended next:** pick up W3-04 (already open) or W3-10 after filing
```

If drafting does not unblock anything soon, say what human decision is needed
(e.g. "choose Wave 3 tranche 2 scope" or "confirm L4 milestone timing").
