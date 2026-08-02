---
name: draft-migration-issues
description: >-
  Path C of pick-up-work-item (fallback when no PRs need prep, no ready issue,
  and backlog is not saturated). Drafts implementable ~200 LOC child issues with
  local specs — only allowlisted leaf/pure slices with a clear L0/L1/L2 path.
  Favor wide parallel graphs over deep blocked_by chains. Do NOT file when a
  single lane is saturated, or scope is HAL/Wave 4. Completes the pick-up
  workflow — do NOT select or implement a newly drafted issue in the same run.
  Do NOT use to duplicate existing open issues.
metadata:
  parent-skill: pick-up-work-item
  path: C
---

# Draft Migration Issues

When **no open issue is ready**, figure out what tickets are missing to keep the
migration moving, then draft them in the repo's issue format.

**Draft only when there is a clear implementation path.** Do not file issues that
lack a spec, depend on Wave 4 HAL, or cannot pass L0 (+ L1/L2 per slice) without
new unplanned infra. **Favor tickets that enable parallel work** — see
**Favor wide graphs over deep chains** below.

## Favor wide graphs over deep chains (mandatory — read first)

This is a **large port** across dozens of `core/` translation units. Most slices
can be implemented **concurrently** by different agents. The tracker should look
like a **wide** graph (many independent lanes), not a **deep** chain (everything
waiting on everything else).

```text
BAD (deep chain):  W3-01 → W3-02 → … → W3-129   (one lane, one agent at a time)

GOOD (wide graph): rtw_rf.c lane ──┐
                   rtw_recv.c lane ├── many ready at once
                   rtw_mlme.c lane ─┘
                   (chain only *within* a lane when technically required)
```

**Default assumption:** a new slice in a **different C file** or **non-overlapping
Rust module** is **not** blocked by unrelated slices. Do **not** set
`blocked_by: [W3-NN-1]` just because the previous ID was filed in the same batch.

### When to use `blocked_by` (narrow — real deps only)

Add a blocker **only** when the slice cannot compile or pass gates without the
predecessor:

| Use `blocked_by` | Example |
|----------------|---------|
| **Same C file**, later slice calls Rust introduced in an earlier slice | W3-20 uses `rtw_get_center_ch` from W3-19 in `rtw_rf.c` |
| **Same file**, Makefile object swap follows implementation | Part 2 swaps the `.o` only after Part 1's Rust exists |
| **Missing shared infra** | `blocked_by: [T5]` harness, `[A2]` domain types |
| **True cross-file API** | Slice B imports a `pub fn` that only exists after slice A lands |

If the coupling is soft ("nice to have same PR stack"), put it in **Notes** — do
**not** add a `blocked_by` edge.

### When NOT to use `blocked_by`

| Do **not** block on | Why |
|---------------------|-----|
| **Different C file** / module | `rtw_rf.c` vs `rtw_rm_util.c` vs `rtw_mlme.c` — independent |
| **Issue ID sequence** | `W3-53` is not blocked by `W3-52` because numbering |
| **Epic tranche / table order** | Tranches are planning batches, not pipelines |
| **Unrelated wave child** | Prior slice closed is not required unless API-coupled |
| **"File the whole TU in order"** | Split a TU into parallel part-1 slices when functions are independent |

**Wrong pattern (seen in production):** filing W3-41…W3-83 as
`blocked_by: [previous ID]` across `rtw_ieee80211.c`, `rtw_recv.c`, `rtw_mlme.c`,
and `rtw_rf.c` — creating an 80-deep chain when most slices could run in parallel.

### Draft batches for parallelism

When filing **5–15** issues in one session:

1. **Pick multiple independent lanes** — different C files or Rust modules (e.g.
   one RF slice, one recv slice, one mlme slice, one `T*` harness).
2. **Root each lane** at shared infra only — `blocked_by: []`, or `[T5]`, `[A2]`,
   etc. — **not** the previous issue ID in the batch.
3. **Chain only inside a lane** when same-file technical deps exist (part 1 → part 2).
4. **Cap chain depth per lane** — prefer ≤3 sequential issues per C file before
   parallel part-1 slices in the same file.
5. **Report parallelism** — how many drafted issues are immediately ready vs chained.

### `blocked_by` audit (before filing)

For every proposed `blocked_by` entry, ask:

> Can this slice build and pass L0/L1/L2 against current `master` (or an open dep
> PR) **without** that predecessor?

| Answer | Action |
|--------|--------|
| **Yes** | Remove the blocker (or use infra-only deps like `T5`) |
| **No** | Keep it; document the **specific** function/API coupling in Notes |

## When NOT to draft (check first — mandatory)

Stop and report instead of filing when **any** of these hold:

| Condition | Action |
|-----------|--------|
| Chain head has **no accessible code** (open issue, no implementing PR) | True frontier — Path B cannot start here; Path C only if tranche gap |
| Chain head has **its own** open PR and **no downstream** issue is ready | Prep/babysit existing PRs (Path A) — do not duplicate the chain head |
| **≥15 open children** already filed behind the same **single-lane** chain head | That lane's backlog saturated — draft **other parallel lanes** instead, or implement/stack |
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

Do **not** default to **10–20** issues in one **deep chain** when the tracker
already holds a long single-lane `blocked_by` sequence. Prefer **new parallel
lanes** over extending an existing chain.

## What kinds of issues to create (allowlist)

Each new issue must be an **implementable ~200 LOC C→Rust slice** with:

- **Concrete scope** — named C functions in a `core/` (or approved `core/crypto/`)
  file, not "translate rtw_p2p.c" without a function list
- **Rust target** — existing or new `rust/<module>.rs` module; Makefile swap path clear
- **Gates in Acceptance** — L0 + L1 on swap; L2 when behavior is testable on host
- **Local draft markdown** — `docs/rust-migration/issues/<file>.md` with YAML
  frontmatter **before** filing to GitHub (no GitHub-only bodies)
- **`blocked_by`** — **real** predecessor slices only (see **Favor wide graphs**);
  empty `[]` is normal for independent lanes; not epic parent

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

### Batch shape (required — wide over deep)

Before writing specs, scan the active epic's deferred list and remaining `core/`
translation units. **Prioritize parallel lanes**, not one long chain:

1. List **C files / modules** with unfiled or under-covered scope.
2. Pick **3–6 independent lanes** (different files) when possible.
3. Within each lane, split into ~200 LOC slices; **chain only** when same-file
   functions truly depend on each other.
4. Draft **5–15** issues total across lanes (see **When NOT to draft** and the
   ≥15 per-lane cap).

**Example batch (good):**

| ID | C file | `blocked_by` | Ready when |
|----|--------|--------------|------------|
| W3-90 | `rtw_rf.c` part 1 | `[]` or `[T4]` | immediately |
| W3-91 | `rtw_recv.c` leaf | `[]` | immediately |
| W3-92 | `rtw_mlme.c` BSSID | `[]` | immediately |
| W3-93 | `rtw_rf.c` part 2 | `[W3-90]` | after W3-90 (same file) |

**Example batch (bad):** W3-90 → W3-91 → W3-92 → … all `blocked_by: [previous]`
across unrelated files.

Minimum bar when a tranche gap is confirmed (see **When NOT to draft**):

1. Cover C files from the epic's deferred list that have **clear ~200 LOC leaf
   slices** and an **allowlisted** type above.
2. **Maximize parallel roots** — most new issues should have `blocked_by: []` or
   infra-only deps; chain **only within** a lane when technically required.
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

Use existing children as templates (e.g. `wave3-04-security-type-str.md`).

**Independent lane (preferred default):**

```yaml
---
title: "[W3-91] Translate rtw_recv.c — leaf helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-91
epic: E05
blocked_by: []
estimate_loc: 200
---
```

**Same-file sequence (only when technically required):**

```yaml
---
title: "[W3-20] Translate rtw_rf.c — channel/frequency conversion"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-20
epic: E05
blocked_by: [W3-19]   # W3-19 introduces rtw_get_center_ch used here
estimate_loc: 200
---

## Goal

Port … from [`core/...`](../../../core/...) to [`rust/...`](../../../rust/...).

## Notes

- Why this slice / split here
- What stays in C until a later issue
- **Dependency rationale:** name the specific function/API if `blocked_by` is non-empty

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
**Gap:** Wave 3 tranche 2 complete; `rtw_mlme.c` and `rtw_iol.c` have no open children.

**Drafted (4 issues, 3 parallel lanes):**
| ID | File | Title | Blocked by | Parallel lane |
|----|------|-------|------------|---------------|
| W3-90 | wave3-90-….md | rtw_rf.c part 1 | `[]` | rf (ready now) |
| W3-91 | wave3-91-….md | rtw_recv.c leaf | `[]` | recv (ready now) |
| W3-92 | wave3-92-….md | rtw_mlme.c BSSID | `[]` | mlme (ready now) |
| W3-93 | wave3-93-….md | rtw_rf.c part 2 | W3-90 | rf (chained in-lane) |

**Parallelism:** 3 of 4 immediately ready; 1 same-file follow-up.

**Filed on GitHub:** yes — #300–#303 (example)

**For a future pick-up:** W3-90, W3-91, W3-92 are independently selectable.
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
