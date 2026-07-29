---
name: draft-migration-issues
description: >-
  Path C of pick-up-work-item (fallback when no open PRs and no ready issue).
  Analyzes in-progress epics and drafts a wave of new ~200 LOC child issues
  (typically 10–20 tickets) as markdown specs. Can file via file-issues.sh.
  Completes the pick-up workflow — do NOT select or implement a newly drafted
  issue in the same run. Do NOT use to duplicate existing open issues.
metadata:
  parent-skill: pick-up-work-item
  path: C
---

# Draft Migration Issues

When **no open issue is ready**, figure out what tickets are missing to keep the
migration moving, then draft them in the repo's issue format.

**Draft a meaningful wave, not a token sample.** When the active wave has **zero**
open implementable children (frontier exhausted), draft **10–20 issues** based on
repo state and migration direction — enough backlog for several pick-up cycles.
Do **not** stop after 1–2 tickets unless the remaining scope is genuinely tiny
(e.g. one small C file left in the wave).

| Situation | How many to draft |
|-----------|-------------------|
| Frontier empty — new tranche / wave slice needed | **10–20** issues (default) |
| Partial gap — a few large TUs still lack children | **5–10** issues for that TU cluster |
| Single deferred helper named in an epic | **1–3** issues only when scope is truly that small |

**This step completes the pick-up workflow.** After drafting and/or filing,
report results and **stop**. Do not re-run `select-ready-issue`, open a PR stack,
or start implementation — even if a newly filed issue appears unblocked.
Wait for an explicit follow-up from the user or a new pick-up run.

## 1. Diagnose the gap

Answer these questions from epics, README, and GitHub:

| Question | Sources |
|----------|---------|
| Which epic/wave is active? | `epic-*.md`, open `[Epic]` issues on GitHub |
| Which children are done vs open? | `gh issue list` / `gh issue view` (state) |
| What is blocked and why? | `## Tracking` footers on GitHub, open PRs |
| What large units lack child slices? | Wave 4+ epics, oversized C files |
| What test infra is missing? | `E10`, `test-*.md`, CI workflow gaps |

```bash
# Epic progress (sub-issues on GitHub)
gh issue view 68 --json title,body,state
gh issue view 68 --json subIssues,subIssuesSummary 2>/dev/null || true
```

Common gap patterns in this repo:

- **Wave tranche not filed** — epic lists deferred scope but no open `W3-*` issues on GitHub
- **Oversized C TU** — needs splitting like `wave2-07` / `wave2-08` (part 1/2)
- **Missing L2 harness** — translation issue blocked until `T4`/`T5`-style work exists
- **Domain types** — `A2`/`A3` not done but wave children list them in `blocked_by`
- **Deferred scope** — epic Notes say "file separate issues when…"

## 2. Decide what to draft

### Batch size (required)

Before writing specs, scan the active epic's deferred list and remaining `core/`
translation units. Split each large TU into ~200 LOC function groups (same
pattern as W2-07/W2-08 part 1/2, or W3-10…W3-18 tranche 2). **Aim to exhaust
the obvious next tranche in one drafting session** — e.g. all leaf helpers in
`rtw_rf.c`, then `rtw_ieee80211_rest`, then `rtw_rm_util`, etc.

Minimum bar when the frontier is empty:

1. Cover **every** C file named in the epic's "deferred / tranche 2" list that
   has clear ~200 LOC slices.
2. Chain issues with `blocked_by` in dependency order (W3-N → W3-N+1).
3. Stop only when the next files are HAL-heavy with no obvious leaf slices, or
   you have filed **≥10** issues for the tranche (cap at ~20 unless scope is huge).

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

Increment IDs beyond the highest in `ISSUE-MAP.md` (or GitHub titles `[W3-NN]`).

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
   [`file-issues.sh`](../../../docs/rust-migration/issues/file-issues.sh) if not
   already listed
2. Run from repo root:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

The script is idempotent — it skips IDs already in `ISSUE-MAP.md`.

3. Verify new issues on GitHub (`gh issue view`); do **not** add status rows to README
4. Commit draft markdown + `ISSUE-MAP.md` changes (if filed)

**Ask the user** before filing to GitHub if they only wanted local drafts.

## 5. Sub-issue linking (GitHub)

**Not available in current `gh` (2.91.0).** `--add-parent` / `--parent` are not
supported yet. Skip this step unless you have a newer `gh` or use the GraphQL API
manually. When supported, link sub-issues to the epic parent:

```bash
# Future / newer gh only:
gh issue edit <child-number> --add-parent <epic-number>
# or during create:
gh issue create --parent <epic-number> ...
```

Sub-issues roll up progress on the parent epic in GitHub Projects.

## 6. Report

```markdown
**Gap:** Wave 3 tranche 1 complete; no W3-10+ filed; frontier empty.

**Drafted (22 issues):**
| ID | File | Title | Blocked by |
|----|------|-------|------------|
| W3-10 | wave3-10-….md | … | W3-09 |
| … | … | … | … |
| W3-31 | wave3-31-….md | … | W3-30 |

**Filed on GitHub:** yes — #179–#200

**Unblocks:** W3-10 ready after W3-09 (closed); chain through W3-31

**For a future pick-up (not this run):** W3-10 is the next implementable issue.
```

Report the **total count** of drafted/filed issues. If you filed fewer than 10
and the epic still lists large deferred TUs, explain why (e.g. "only
`rtw_mem.c` left in wave — 1 issue sufficient").

## 7. Completion criteria

The job is **done** when one of these is true:

| Outcome | Done when |
|---------|-----------|
| Local drafts only | Markdown specs committed; user informed |
| Filed on GitHub | `file-issues.sh` run, `ISSUE-MAP.md` updated, verified on GitHub |
| Nothing to draft | Gap analysis explains why; human decision documented |

**Never** continue to `plan-stacked-prs` or `implement-stacked-prs` as part of
this step. Newly created issues are backlog for a **later** session.
