---
name: draft-migration-issues
description: >-
  Path C of pick-up-work-item (fallback when no PRs need prep, no ready issue,
  and backlog is not saturated). Drafts implementable ~200 LOC child issues with
  local specs — only allowlisted leaf/pure slices with a clear L0/L1/L2 path.
  Do NOT file when chain head has no accessible code but backlog is deep, or
  scope is HAL/Wave 4. Completes the pick-up workflow — do NOT select or
  implement a newly drafted issue in the same run. Do NOT use to duplicate
  existing open issues.
metadata:
  parent-skill: pick-up-work-item
  path: C
---

# Draft Migration Issues

When **no open issue is ready**, figure out what tickets are missing to keep the
migration moving, then draft them in the repo's issue format.

**Draft only when there is a clear implementation path.** Do not file issues that
sit behind an in-flight chain head, lack a spec, depend on Wave 4 HAL, or cannot
pass L0 (+ L1/L2 per slice) without new unplanned infra.

## When NOT to draft (check first — mandatory)

Stop and report instead of filing when **any** of these hold:

| Condition | Action |
|-----------|--------|
| Chain head has **no accessible code** (open issue, no implementing PR) | True frontier — Path B cannot start here; Path C only if tranche gap |
| Chain head has **its own** open PR and **no downstream** issue is ready | Prep/babysit existing PRs (Path A) — do not duplicate the chain head |
| **≥15 open children** already filed behind the same chain head | Backlog saturated — implement/merge/stack, do not extend the chain |
| Next slices are **HAL / PHYDM / USB HCI** (`hal/`, `halmac/`, `phydm/`, `hci/`) | Defer to Wave 4 epic — not Path C in Wave 3 |
| Slice needs **new L2 harness** but no `T*` issue exists to add it | Draft a **test-infra** child first (`T*`), not a translation child |
| Slice needs **domain types** (`A2`/`A3`) still open | Finish architecture issues first, or pick a leaf that does not list them |
| Cannot name concrete C functions + target `rust/*.rs` + gates | Do not file — spec is not implementable |
| Duplicate of an open issue | Skip — search `gh issue list` and `ISSUE-MAP.md` |

**Wrong pattern (seen in production):** W3-39 in-flight with open PR stack, while
80+ open W3-40…W3-129 issues already filed behind it — yet another agent run
files W3-98…W3-129 again. That adds tickets with **no new implementable surface**.

**Also wrong:** reporting "all tickets blocked" because W3-39 is open — when W3-39
has an open PR, W3-40+ are **ready for Path B** (stack on the W3-39 PR branch).
Only the chain-head issue itself is excluded while its PR is open.

## When TO draft

| Situation | How many to draft |
|-----------|-------------------|
| Active tranche closed on GitHub and **no** open children filed for the next ~200 LOC slices | **5–15** issues for that tranche only |
| Epic lists deferred C files and **zero** open children cover them | **Up to 10** leaf slices with L0/L2 path |
| Missing test harness blocks a **named** translation slice | **1–3** `T*` issues, not translation children |
| Single small remainder (one C file, &lt;3 slices left in wave) | **1–3** issues |

Do **not** default to **10–20** issues when the tracker already holds a long
`blocked_by` chain for the active wave.

## What kinds of issues to create (allowlist)

Each new issue must be an **implementable ~200 LOC C→Rust slice** with:

- **Concrete scope** — named C functions in a `core/` (or approved `core/crypto/`)
  file, not "translate rtw_p2p.c" without a function list
- **Rust target** — existing or new `rust/<module>.rs` module; Makefile swap path clear
- **Gates in Acceptance** — L0 + L1 on swap; L2 when behavior is testable on host
- **Local draft markdown** — `docs/rust-migration/issues/<file>.md` with YAML
  frontmatter **before** filing to GitHub (no GitHub-only bodies)
- **`blocked_by`** — only real predecessor slices (not epic parent)

### Good issue types

| Type | Example | Notes |
|------|---------|-------|
| Pure / leaf helpers | W3-39 recv counters, W3-45 VHT MCS tables | Host L2 oracles feasible |
| Security / crypto wrappers | W3-01 swcrypto | T5 harness exists |
| IE parse / string helpers | W3-03, W3-04 | T5 / `tests/host/ie/` |
| Chplan / RF tables | W2-17…W2-20, W3-19 | T4 chplan harness |
| Test infra (`T*`) | host harness, CI gate | Unblocks translation children |
| Architecture (`A*`) | domain types | Unblocks `blocked_by: [A2]` children |

### Do NOT create (blocklist)

| Type | Why |
|------|-----|
| HAL register/MMIO paths | Wave 4 (`E06`); needs HAL bindgen + L3 |
| `rtw_ioctl_*`, `rtw_mp` manufacturing | Heavy adapter/HAL coupling |
| `odm_*`, `phydm_*`, beamforming init | PHYDM; no Rust seam yet |
| `rtw_sdio` / `rtw_usb` HCI | Wave 4 / Wave 5 glue |
| `rtw_eeprom` bit-bang without I2C harness | Hardware timing; no L2 path |
| `rtw_btcoex` / `rtw_beamforming` cmd paths | Cross-layer; defer until cmd/mlme slices land |
| Epic-only tracking issues | Use existing `[Epic]` parents |
| Speculative "file the rest of core/" batches | Split by function group with gates |

If the only remaining scope is blocklisted, report **human decision needed** —
do not file placeholder tickets.

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
| What is blocked and why? | `## Tracking` footers on GitHub, open PRs — remember: open dep PR **unblocks** dependents |
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
pattern as W2-07/W2-08 part 1/2, or W3-10…W3-18 tranche 2). Draft the next
tranche slice in one session (typically **5–15** issues per **When NOT to draft**
and the ≥15 open-child cap below) — e.g. leaf helpers in `rtw_rf.c`, then
`rtw_ieee80211_rest`, then `rtw_rm_util`, etc.

Minimum bar when a tranche gap is confirmed (see **When NOT to draft**):

1. Cover C files from the epic's deferred list that have **clear ~200 LOC leaf
   slices** and an **allowlisted** type above.
2. Chain issues with `blocked_by` in dependency order — but **do not** extend a
   chain that already has ≥15 open children behind the head.
3. Commit local markdown for **every** issue before filing; cap at ~15 per session.

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

**Drafted (N issues):**
| ID | File | Title | Blocked by |
|----|------|-------|------------|
| W3-10 | wave3-10-….md | … | W3-09 |
| … | … | … | … |

**Filed on GitHub:** yes — #179–#181 (example)

**Unblocks:** W3-10 ready after W3-09 (closed)

**For a future pick-up (not this run):** W3-10 is the next implementable issue.
```

Report the **total count** of drafted/filed issues. If the count is small and the
epic still lists large deferred TUs, explain why (e.g. "only `rtw_mem.c` left in
wave — 1 issue sufficient").

## 7. Completion criteria

The job is **done** when one of these is true:

| Outcome | Done when |
|---------|-----------|
| Local drafts only | Markdown specs committed; user informed |
| Filed on GitHub | `file-issues.sh` run, `ISSUE-MAP.md` updated, verified on GitHub |
| Nothing to draft | Gap analysis explains why (blocked head with no accessible code, saturated backlog, HAL-only scope, or blocklist) |

**Never** continue to `plan-stacked-prs` or `implement-stacked-prs` as part of
this step. Newly created issues are backlog for a **later** session.
