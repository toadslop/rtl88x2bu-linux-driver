---
name: implement-stacked-prs
description: >-
  Path B step 4 of pick-up-work-item. Implements an approved stacked-PR plan
  one PR at a time: branch, code, mandatory ≤250-line diff gate, gates, push,
  open PR ready for review (not draft) with stack base, then babysit until CI is
  green. Auto-applies after plan-stacked-prs approval. Do NOT use without an
  approved plan or for PRs unrelated to the selected issue.
metadata:
  parent-skill: pick-up-work-item
  path: B
  step: 4
  requires-skill: babysit
---

# Implement Stacked PRs

Execute the approved plan from **`plan-stacked-prs`** — one PR at a time, bottom
of the stack first.

**PR size is a blocking gate.** Target ~200 changed lines; **never open a PR
above 250** (insertions + deletions vs stack base). See
[`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#pr-size-limit-mandatory--read-first).

## Before starting

- [ ] Plan approved (explicit user OK or "go ahead and implement")
- [ ] Issue selected and spec read
- [ ] **Stack base resolved** — `master` or dependency PR branch from selection report
- [ ] Stack base branch fetched and up to date
- [ ] No open PR already covers PR1 of this stack (avoid duplicates)

```bash
# When all blocked_by deps are closed:
git fetch origin master
git checkout master && git pull origin master

# When selection reported a dependency PR branch (e.g. cursor/w3-39-…):
git fetch origin <dep-pr-branch>
git checkout -b cursor/<name>-<suffix> origin/<dep-pr-branch>
```

## Per-PR loop

Repeat for each row in the plan table (PR1 → PR2 → …):

### 1. Branch

```bash
# PR1 — base from plan (master OR dependency PR branch)
git fetch origin <pr1-base>
git checkout -b cursor/<name>-<suffix> origin/<pr1-base>

# PR2+ — base previous PR branch in this stack
git fetch origin cursor/<prev-branch>
git checkout -b cursor/<name>-<suffix> origin/cursor/<prev-branch>
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

### 5. Commit and push

Re-run the step 3 size gate (same commands) after gate fixes — L0/L2 repair
edits must not push the PR over 250.

```bash
git add -A
git commit -m "<type>: <short description> (#<issue>)"
git push -u origin HEAD
```

Reference the GitHub issue in the commit message (`#115`, `W3-04`).

### 6. Open stacked PR (ready for review — not draft)

Open each PR in **open** (ready-for-review) state so CI and review can start
immediately. Do **not** use `--draft` or `draft: true`.

Prefer `ManagePullRequest` `create_pr` with `draft: false` (default). Fallback
for local shells:

```bash
gh pr create --base <stack-parent> --head <branch> --title "<title>" --body "<body>"
# Do not pass --draft
```

| Field | PR1 | PR2+ |
|-------|-----|------|
| `base_branch` / `--base` | `master` **or** dependency PR `headRefName` from plan (see [`plan-stacked-prs`](../plan-stacked-prs/SKILL.md#pr1-base-when-dependencies-are-on-open-prs-mandatory)) | previous PR head branch |
| `branch_name` / `--head` | current head | current head |
| `draft` | `false` / omit | `false` / omit |
| `title` | from plan | from plan |
| `body` | link issue, gates run, stack position | + "Stacked on #N" |

PR body should include:

- **`@toadslop`** — maintainer notification (required; near the top of the body)
- `Closes #N` or `Part of #N` (use **Closes** only on the final PR of the stack)
- Gates executed
- Stack diagram or "PR 2 of 3 — base: `cursor/...`"
- **Measured Δ** — lines changed vs base (from step 3)

### 7. Update tracking

- Add `In-flight: <branch>` to the issue via comment if not already noted
- Do not close the issue until the **last** PR merges and acceptance is met

### 8. Babysit the PR you just opened

**Default: per-PR babysit.** After opening a PR, babysit it until required CI
checks pass **before** opening the next PR in the stack. This catches base-layer
failures early and matches step 9's per-PR loop.

1. Load Cursor's built-in **`babysit`** skill when available; otherwise fix CI
   failures, push to the same branch, and re-poll `gh pr checks <number>`.
2. Address blocking review feedback if any arrives during babysit (same rules as
   `prepare-pr-for-merge` manual `babysit` fallback).
3. Loop until checks are green or you report a blocker — only then continue to
   the next PR in the plan.

Path B pick-up ends after the final PR opens and babysit passes — full merge prep
(`prepare-all-prs-for-merge`) runs on a **future** pick-up once these PRs are
open.

### 9. Continue the stack (mandatory — no partial stops)

Path B pick-up and other autonomous runs have **no human on the line**. Once you
start implementing a planned stack, you **must** reach one of these end states
before ending the session:

| End state | When |
|-----------|------|
| **`stack complete`** | Every row in the plan table has an open PR; babysit passed on each |
| **`stack partial — tracked`** | A genuine blocker prevents the next PR; remaining rows are filed as follow-up issues (see below) |
| **Plan revised** | Scope grew past 250 lines or the split changed; return to `plan-stacked-prs`, update the table, then continue implementing or file tracking |

**Forbidden:** Opening PR1 and PR2 then asking "Should I implement PR3?", ending
with "Next: implement PR3" as a suggestion, or otherwise stopping mid-stack
without either finishing or filing tracking. Autonomous agents must not defer
remaining PRs to a hypothetical future confirmation.

**Per-PR loop (default):**

- After babysit passes on the current PR, **immediately** implement the next PR
  in the stack — same session, no pause for confirmation.
- Continue until the plan table is fully implemented or you hit a blocker that
  requires filing tracking.
- **Pause** only when the user **explicitly** asked for incremental delivery in
  this session (rare; not the default for Path B pick-up). Babysit still applies
  to the PR you opened before pausing.
- After the **final** PR opens and babysit passes, summarize the full stack with
  links and mark workflow end **`stack complete`**.

### 10. When you cannot finish the stack (mandatory tracking)

If a blocker (gate failure you cannot fix, missing harness, dependency reopened,
ambiguous spec, exhausted CI retries) prevents completing **all** remaining plan
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
| Do not retarget bases until `prepare-pr-for-merge` | that skill owns landing on `master` |
| Rebase only when necessary to fix conflicts | prefer adding a fix-up commit on the stack |
| **Every PR ≤ 250 changed lines (target ~200)** | enforced in step 3 before commit — non-negotiable |

## When implementation fails

| Situation | Action |
|-----------|--------|
| Diff **> 250** lines at size gate | Split scope or return to `plan-stacked-prs` — **never** open an oversized PR |
| Scope bigger than planned | Revise plan (return to `plan-stacked-prs`), then continue the stack — do not cram |
| Blocked by missing harness | Implement harness PR first, **or** file follow-up issue(s) + parent comment and end **`stack partial — tracked`** |
| Gate fails and cannot be fixed | File follow-up issue(s) for remaining rows; end **`stack partial — tracked`** — do not ask to continue |
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

**Next:** next pick-up will Path A (`prepare-all-prs-for-merge`) when PRs are open
```
