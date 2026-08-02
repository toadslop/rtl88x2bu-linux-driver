---
name: triage-open-issues
description: >-
  Path B/C step 1 of pick-up-work-item. Audits open rust-migration GitHub Issues,
  finds ones already completed (merged PR, code landed, acceptance met), and
  closes them with evidence. Auto-applies as part of pick-up-work-item triage.
  Do NOT use standalone to close issues the user explicitly wants kept open.
metadata:
  parent-skill: pick-up-work-item
  path: B/C
  step: 1
---

# Triage Open Issues

Close **open GitHub issues that are already done** before selecting new work.
This keeps the tracker honest and prevents duplicate effort.

**GitHub Issues are the only status tracker** — do not update local markdown
tables or README rows to record open/closed state.

## 1. Gather open issues

List open migration issues (adjust limit as needed):

```bash
gh issue list --repo "$(gh repo view --json nameWithOwner -q .nameWithOwner)" \
  --label rust-migration --state open --limit 200 \
  --json number,title,labels,body,state
```

Cross-check against recently merged PRs and code on `master` — an open GitHub
issue with landed acceptance criteria is a triage candidate even if local draft
specs still exist in the repo.

## 2. Evidence that an issue is complete

An issue is **done** when **all** of the following hold:

| Check | How to verify |
|-------|---------------|
| Acceptance criteria met | Read the issue body (and local draft spec if linked) |
| Code landed on `master` | Grep for the Rust object / Makefile swap / harness named in the issue; or inspect merged PR diff |
| Blockers cleared | Every `blocked_by` issue in the `## Tracking` footer is `CLOSED` on GitHub (merged to `master` — an open implementing PR satisfies deps for **starting** downstream work per [`select-ready-issue`](../select-ready-issue/SKILL.md), but does **not** clear blockers for **closing** this issue) |
| Gates green for that slice | L0 (+ L1/L2 per issue spec) — check CI or run locally if unsure |

**Strong signals (any one plus acceptance):**

- Merged PR whose title or body references the draft ID (e.g. `W3-03`, `#114`)
- Issue body still says `In-flight: cursor/...` but that branch merged

**Do not close** when:

- An open PR still implements the issue
- Only part of a multi-PR issue is merged (close only when the **full** issue acceptance is met)
- You cannot verify gates — investigate first

## 3. Cross-check PRs

```bash
# Merged PRs mentioning an issue number
gh pr list --repo "$(gh repo view --json nameWithOwner -q .nameWithOwner)" \
  --state merged --search "114" --json number,title,mergedAt

# Open PRs that might be in-flight for an issue
gh pr list --state open --json number,title,headRefName,body
```

Search by draft ID (`W3-03`), GitHub number (`#114`), and branch name from the issue Notes.

## 4. Close with a comment

For each issue confirmed done:

```bash
gh issue close <number> --comment "Closing: <evidence>.
- Merged: PR #N (or commit on master)
- Acceptance: <brief checklist>
- Gates: L0/L1/L2 verified via <CI or local run>"
```

Do **not** mirror closed state into `README.md` or other repo files. GitHub is
authoritative.

**Epics (`[Epic]` titles):** close only when **all** planned children for that
wave/phase are done and the epic's verification gate is met (see the epic's
`epic-*.md` file). Otherwise leave the epic open.

## 5. Report triage results

Reply with a short table:

| Issue | Action | Evidence |
|-------|--------|----------|
| `#114` W3-03 | closed | PR #61 merged, `rust/rtw_ieee80211.rs` on master |
| `#115` W3-04 | left open | no merged PR; acceptance not met |

If nothing to close, say so explicitly and continue to **select-ready-issue**.
