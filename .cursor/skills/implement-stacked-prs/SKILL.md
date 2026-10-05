---
name: implement-stacked-prs
description: >-
  Path B step 4 of pick-up-work-item. Implements every row of the approved plan
  (≤7 PRs per issue) in **one agent session** — no PR1 plus a GitHub issue for
  the rest unless a documented §10 blocker. Prefer `gh stack` for linkage; local
  build-then-submit is recommended, not required. Auto-applies after plan-stacked-prs
  approval. Do NOT use without an approved plan or for PRs unrelated to the selected issue.
metadata:
  parent-skill: pick-up-work-item
  path: B
  step: 4
  requires-skill: babysit
---

# Implement Stacked PRs

Execute the approved plan from **`plan-stacked-prs`** — implement **every row**
bottom → top in **one pick-up session**. The failure mode to avoid is **not**
"when to run `gh stack submit`" — it is **stopping after PR1** (or PR1…k) and
filing a tracking issue for the remaining plan rows without a §10 blocker.

Use the **Stack CLI** (`gh stack`) so layers appear as a **linked GitHub stack**.
**Recommended:** build all layers on branches first (`gh stack init` / `add`),
gate each layer, then **`gh stack submit --auto --open` once** before babysit.
**Also valid in the same session:** publish layers incrementally (`submit` /
`link` / `create_pr` as you go) as long as you **finish every plan row** and
end with a single linked stack — do not defer unimplemented rows to a follow-up
issue.

**PR size is a blocking gate.** Target ~200 changed lines; **never open a PR
above 250** (insertions + deletions vs stack base). See
[`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#pr-size-limit-mandatory--read-first).

## GitHub Stack CLI (`gh stack`) — mandatory

Stacked work must appear on GitHub as a **linked stack**, not a bag of unrelated
PRs with correct `base` branches only.

```bash
gh extension install github/gh-stack   # idempotent; required before submit/link
```

| Phase | Command |
|-------|---------|
| Local tracking while building | `gh stack init --base <pr1-trunk> <pr1-branch>` then `gh stack add <pr2-branch>` … |
| Adopt existing branches in one step | `gh stack init --base <pr1-trunk> <branch1> <branch2> …` (bottom → top) |
| **Publish** (push + create/update PRs + stack metadata) | **`gh stack submit --auto --open`** (once at end, or again after each new layer) |
| PRs already exist but unlinked | `gh stack link <pr1#> … <prN#> --base <pr1-trunk> --open` or `gh stack link <branch1> … --base … --open` |
| Verify | `gh stack view` / `gh stack view --short` |

Docs: [Stacked PRs CLI commands](https://docs.github.com/en/pull-requests/reference/stacked-prs-cli-commands),
[quickstart](https://docs.github.com/en/pull-requests/get-started/stacked-prs-quickstart).

**Linkage (multi-PR plan):** however you publish, the session must end with
**all** plan layers on GitHub in **one** stack (`gh stack view`). Avoid orphan
PRs that never get `link`ed. Enrich bodies with `ManagePullRequest` `update_pr`
(or `gh pr edit`) so each layer has `@toadslop`, gates, and measured Δ.

**Single-PR plan (one table row):** `gh stack submit` still works; or
`ManagePullRequest` `create_pr` with `draft: false` — no `link` needed.

**Appending to an existing GitHub stack:** `gh stack link <stack-number> <new-pr#> --open`
(do not re-list the whole stack).

Do **not** retarget bases to `master` while PRs stay GitHub-stacked — see
[`prepare-pr-for-merge`](../prepare-pr-for-merge/SKILL.md).

## Before starting

- [ ] Plan approved (explicit user OK or "go ahead and implement")
- [ ] Issue selected and spec read
- [ ] **Stack base resolved** — `master` or dependency PR branch from selection report
- [ ] Stack base branch fetched and up to date
- [ ] No open PR already covers PR1 of this stack (avoid duplicates)
- [ ] `gh extension install github/gh-stack` succeeded

```bash
# When all blocked_by deps are closed:
git fetch origin master
git checkout master && git pull origin master

# When selection reported a dependency PR branch (e.g. cursor/w3-39-…):
git fetch origin <dep-pr-branch>
git checkout -b cursor/<name>-<suffix> origin/<dep-pr-branch>
```

## Per-layer loop (repeat until the plan table is done)

Repeat for each row in the plan table (PR1 → PR2 → …) **in the same session**.
Do **not** end the run after an early row and file a GitHub issue for the rest
(see §9–10).

### 1. Branch + stack tracking

```bash
# PR1 — base from plan (master OR dependency PR branch)
git fetch origin <pr1-base>
git checkout -b cursor/<name>-<suffix> origin/<pr1-base>
gh stack init cursor/<pr1-branch> --base <pr1-base>   # first layer only

# PR2+ — base previous PR branch in this stack
git fetch origin cursor/<prev-branch>
git checkout -b cursor/<name>-<suffix> origin/cursor/<prev-branch>
gh stack add cursor/<current-branch>
```

Cloud agents: branch names must match `cursor/<descriptive-name>-e465` when that
suffix is configured for the run.

### 2. Implement

- Follow the draft spec and per-PR plan scope
- Match existing code style in `rust/` and C shims
- **Characterize C → freeze tests → port** (per [`architecture.md`](../../../docs/rust-migration/architecture.md))
- Minimal diff — no drive-by refactors

### 3. PR size gate (mandatory — before commit)

**Do not commit or open a PR until this passes.** This is the enforcement step
that prevents 400–600 line PRs.

```bash
BASE=<stack-base>   # master for PR1, or previous PR branch for PR2+
git fetch origin "$BASE"
git add -A   # stage untracked files (new rust/*.rs, harness files) before measuring
# Merge-base → working tree (no ..HEAD): includes staged + unstaged changes
STAT=$(git diff --shortstat "$(git merge-base HEAD "origin/$BASE")")
echo "$STAT"
# Parse insertions + deletions; sum must be ≤ 250
```

| Result | Action |
|--------|--------|
| **≤ 250** changed lines | Continue to gates (step 4) |
| **> 250** | **STOP** — do not commit, do not open PR. Split scope: move overflow to the next planned PR (or return to `plan-stacked-prs` to revise the stack) |
| Growing while implementing | Pause, re-estimate, split before pushing |

Report the measured Δ in the PR body (e.g. `**Size:** 187 lines changed (Δ)`).

### 4. Verify gates

Run applicable gates from [`AGENTS.md`](../../../AGENTS.md):

```bash
export LIBCLANG_PATH=/usr/lib/llvm-18/lib
make clean && make KDIR=/opt/linux LLVM=1 -j"$(nproc)"
./scripts/ci/verify-ko-probes.sh 88x2bu.ko
```

| Gate | When |
|------|------|
| **L0** | Every PR that touches module build |
| **L1** | Every C→Rust object swap — `make rust-check-symbols OLD=… NEW=…` |
| **L2** | Crypto / chplan / security / wlan harness — `make -C tests/host/crypto all` (or scoped target) |
| **L3** | Init / load path changes — QEMU recipe in [`dev-environment.md`](../../../docs/rust-migration/dev-environment.md) |

Do not open a PR with failing gates for its scope.

### 5. Commit (push optional until publish)

Re-run the step 3 size gate (same commands) after gate fixes — L0/L2 repair
edits must not push the layer over 250.

```bash
git add -A
git commit -m "<type>: <short description> (#<issue>)"
# Optional before submit: git push -u origin HEAD
```

Reference the GitHub issue in the commit message (`#115`, `W3-04`). Use plan
titles in commit subjects when helpful — `gh stack submit --auto` uses them for
PR titles.

After each layer passes gates, either continue to the **next plan row** or, if
this was the **last** row, ensure the **whole** stack is on GitHub (step 6).

### 6. Publish on GitHub (`gh stack` — required before session ends)

Before ending Path B, **every** plan row must exist as an open PR in a **linked**
stack. How you get there:

| Approach | When to use |
|----------|-------------|
| **Deferred publish (recommended)** | Implement and gate all rows on branches first; then one `gh stack submit --auto --open` |
| **Incremental publish (same session)** | After a layer is ready, `gh stack submit --auto --open` (or `link` / `create_pr` for that layer), then implement the next row on top — repeat until the plan table is done |

```bash
gh stack submit --auto --open
gh stack view --short
```

`submit` pushes stack branches, creates or updates PRs with chained bases, and
creates/updates **GitHub stack** metadata. Use **`--open`** so PRs are ready for
review (not draft). Non-interactive agents must pass **`--auto`** when using
`submit`.

If branches were already pushed and PRs exist but are not linked:

```bash
gh stack link <branch-or-pr-bottom> ... <branch-or-pr-top> --base <pr1-trunk> --open
gh stack checkout <bottom-pr-number>
gh stack view
```

Record PR numbers from `gh stack view` (or `gh pr list --head <branch>`).

### 6b. PR titles and bodies (after `submit` / `link`)

Auto titles from `submit --auto` are a starting point only. For **each** layer,
set the final title/body (cloud: `ManagePullRequest` `update_pr` with
`branch_name`; shell: `gh pr edit`):

| Field | PR1 | PR2+ |
|-------|-----|------|
| Title | from plan | from plan |
| Base | set by `gh stack` (`master` or dep `headRefName`) | previous layer branch |
| Draft | **false** — never leave stack layers as draft | same |

PR body must include:

- **`@toadslop`** — maintainer notification (required; near the top)
- `Closes #N` or `Part of #N` (use **Closes** only on the **top** PR of the stack)
- Gates executed per layer
- Stack position (e.g. "PR 2 of 3 — base: `cursor/...`")
- **Measured Δ** — lines changed vs that layer's base (from step 3)

**Forbidden:** opening PR2…PRn with `create_pr` while PR1 is unlinked, or ending
the run without `gh stack view` showing a single linked stack.

### 7. Update tracking

- Add `In-flight: <branch>` to the issue via comment if not already noted
- Do not close the issue until the **last** PR merges and acceptance is met

### 8. Babysit

Babysit **every layer** before ending Path B (bottom → top if the whole stack
was published at once; or the layers you published incrementally, then finish
implementing remaining rows, then babysit **all** layers).

1. Load Cursor's built-in **`babysit`** skill when available; otherwise fix CI
   failures, push, and if needed **`gh stack rebase`** / **`gh stack sync`** (see
   [`prepare-pr-for-merge`](../prepare-pr-for-merge/SKILL.md)), then re-poll
   `gh pr checks` on each layer.
2. Address blocking review feedback (same rules as `prepare-pr-for-merge`).
3. Loop until required checks are green on **all** plan rows' PRs or you hit a
   §10 blocker.

**Deferred publish:** local L0/L1/L2 gates during the per-layer loop are the
early signal; CI babysit runs after `submit`/`link`.

**Incremental publish:** you may babysit a layer after publishing it, then
continue implementing the next row in the **same session** — that is not a
partial stop. A partial stop is **ending the session** with unimplemented plan
rows or a follow-up issue for them without §10.

Path B pick-up ends after **all** plan rows are on GitHub, linked, and babysit
passes — full merge prep (`prepare-all-prs-for-merge`) runs on a **future**
pick-up.

### 9. Continue the stack (mandatory — no partial stops)

Path B pick-up and other autonomous runs have **no human on the line**. Once you
start implementing a planned stack, you **must** reach one of these end states
before ending the session:

| End state | When |
|-----------|------|
| **`stack complete`** | Every row in the plan table (≤7 rows per [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#stack-depth-cap-mandatory--read-before-approving-a-plan)) has an open PR; babysit passed on each |
| **`stack partial — tracked`** | Only when [§10 blockers](#10-when-you-cannot-finish-the-stack-mandatory-tracking) apply — **not** because the stack feels long |
| **Plan revised** | Scope grew past 250 lines per layer or the split changed; return to `plan-stacked-prs`, update the table (≤7 rows), then **continue implementing all rows** |

**Forbidden (common agent failure modes):**

- Implementing **PR1 only** (or PR1–2 of N) and filing a GitHub issue to track
  PR2…PRn **without** a blocker from §10.
- Ending with "Next: implement PR3" or asking whether to continue.
- Treating **time**, **token budget**, **slow CI**, or **stack size** (when the
  approved plan has ≤7 rows) as reasons to defer remaining rows.
- **Ending the session** with unimplemented plan rows (even if PR1 is already on
  GitHub).

The approved plan table is a **contract**: if it has three rows, you owe three
open PRs in one linked stack before the run ends — unless §10 applies.

If you discover mid-implementation that the issue truly needs **8+ PRs**, **stop
adding rows to this plan**: finish **all current plan rows** first (`stack
complete`), then split **new** scope into a **separate GitHub issue** (Path C /
`draft-migration-issues` or `file-issues.sh`) for a **future** pick-up — do not
use `stack partial — tracked` to dump already-planned rows into a follow-up issue.

**Per-layer loop (default):**

- After each layer passes gates, **immediately** implement the next row on a new
  branch (`gh stack add`) — **same session**, no pause for confirmation.
- Publish with step 6 (deferred or incremental); update bodies (6b); babysit (8)
  until **all** rows are done.
- Continue until the plan table is fully implemented and on GitHub, or §10
  forces tracking.
- **Pause** only when the user **explicitly** halted this session.
- When every row is open in a linked stack and babysit passes, summarize with
  `gh stack view --short` and mark **`stack complete`**.

### 10. When you cannot finish the stack (mandatory tracking)

#### When partial stacks are allowed (mandatory)

`stack partial — tracked` is **rare**. It is **not** a way to split work across
sessions when the plan was feasible.

| Valid reason (§10) | Invalid — do **not** file follow-up issues for remaining plan rows |
|--------------------|---------------------------------------------------------------------|
| Gate failure you cannot fix after real debugging | "Only did PR1 to get review started" |
| Missing harness / infra **outside** this issue's scope | Approved plan has 3–7 rows but agent stopped early |
| Dependency issue reopened or stack base became inaccessible | Session time, cost, or fatigue |
| Spec ambiguity that needs a **human** decision | Slow or flaky CI (exhaust retries first — see `babysit`) |
| Exhausted CI retry policy on a **blocking** check | Stack "felt big" while still ≤7 PRs |
| User **explicitly** halted implementation this session | Convenience tracking issue instead of implementing |

If the slice needs **8+ PRs**, that should have been caught in
[`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#stack-depth-cap-mandatory--read-before-approving-a-plan)
— split issues **before** coding, not via `stack partial` after PR1.

When a **valid** blocker prevents completing **all** remaining **already-planned**
rows:

1. **Do not ask** whether to continue — file tracking and end with a clear report.
2. **Comment on the parent issue** with in-flight state: PRs opened (with links),
   which plan rows remain, and the blocker.
3. **File a follow-up GitHub issue** for the unimplemented work — one issue per
   remaining plan row, or one umbrella issue if the rows are tightly coupled and
   must land together. Minimum body:

```markdown
**Parent:** #<parent-issue>
**Continues:** stack after <link to last opened PR>
**Remaining from plan:**
- [ ] PR3: <goal> — base `cursor/<pr2-branch>`, est. Δ ~N
**Blocker:** <what stopped implementation>
**Acceptance:** same gates as parent slice; complete rows PR3…PRn from original plan
```

Use `gh issue create` with labels `rust-migration`, the appropriate `wave-*` /
`phase-*`, and `blocked_by` referencing the parent issue or the last merged PR
in the stack. Add a local draft spec under `docs/rust-migration/issues/` when
the remainder is non-trivial (copy the per-PR detail from the plan).

4. End the run with status **`stack partial — tracked`** and link every filed
   follow-up issue in the completion report.

## Stack hygiene

| Rule | Why |
|------|-----|
| Each PR targets its planned base branch | preserves reviewable increments |
| **`gh stack submit` / `link`** so `gh stack view` shows the full plan | End session with unimplemented rows or unlinked PRs |
| Deferred submit (recommended) when it keeps CI noise down | Treat incremental publish in-session as a partial stop |
| `gh stack link` when PRs pre-exist unlinked | leave PRs unlinked on GitHub at handoff |
| Do not retarget bases to `master` while PRs stay GitHub-stacked | `prepare-pr-for-merge`: `gh stack unstack` first if bottom must move to `master` |
| Use `gh stack rebase` / `gh stack sync` for stack-wide updates | manual per-branch rebase of the whole stack |
| After Path A review fixes on the bottom layer, **`gh stack rebase` the full stack** (`prepare-pr-for-merge`) | Push only the bottom branch and leave upper PRs conflicting |
| **Every PR ≤ 250 changed lines (target ~200)** | enforced in step 3 before commit — non-negotiable |

## When implementation fails

| Situation | Action |
|-----------|--------|
| Diff **> 250** lines at size gate | Split scope or return to `plan-stacked-prs` — **never** open an oversized PR |
| Scope bigger than planned | Revise plan (return to `plan-stacked-prs`); if 8+ PRs needed, **split issues** — then implement **all** rows of the revised plan (≤7) |
| Blocked by missing harness | Implement harness PR first if in plan; else **§10 blocker** → follow-up issue(s) |
| Gate fails and cannot be fixed | **§10 only** — follow-up for remaining rows; never stop after PR1 without blocker |
| Plan has 2–7 rows, no §10 blocker | **Must** `stack complete` — filing a continuation issue is forbidden |
| Dependency has no accessible code (open issue, no PR) | Return to `select-ready-issue` — true blocker; file follow-up only if mid-stack |

## Completion report

Use **`stack complete`** or **`stack partial — tracked`** — never an open-ended
"next step" that assumes a human will pick up mid-stack without a filed issue.

```markdown
**Issue:** W3-04 / #115

**Stack status:** stack complete | stack partial — tracked

**Stack opened:**
| PR | Branch | Base | Status |
|----|--------|------|--------|
| #200 | cursor/w3-04a-… | master (or `cursor/w3-39-…` when stacking on open dep PR) | open |
| #201 | cursor/w3-04b-… | cursor/w3-04a-… | open |

**Gates:** L0/L1/L2 green on PR2

**Babysit:** CI green on opened PRs

**Follow-up issues filed:** none | #NNN (PR3 remainder — blocker: …)

**GitHub stack:** published via `gh stack submit` (or `gh stack link`) — include
`gh stack view --short`

**Next:** Path A (`prepare-all-prs-for-merge`) syncs the stack and babysits all
layers until ready for maintainer **`gh stack merge`**
```
