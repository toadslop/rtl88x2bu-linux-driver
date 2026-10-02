---
name: prepare-pr-for-merge
description: >-
  Prepares a pull request (or a linked GitHub PR stack) for merge into master.
  Auto-applies on "prepare PR for merge", "prepare for merge", "get PR ready to
  merge", or similar author tasks. Uses the gh stack extension to link chained
  PRs into a GitHub stack, sync/rebase the stack on master, and babysit until
  every layer is green and reviews are complete (via Cursor's built-in babysit
  skill). Opens a follow-up PR for unresolved reviewer knits. Agents do not
  merge — when the stack is green, report ready for maintainer gh stack merge.
  Do NOT use for reviewing PRs (use pr-review-delivery). For batch prep, use
  prepare-all-prs-for-merge.
metadata:
  requires-skill: babysit
---

# Prepare PR for Merge

Use this skill when the user (or you as the PR author) wants a pull request **ready
to land on `master`** — not merely "address comments", but the full pre-merge
prep: GitHub stack linkage, stack sync/rebase on `master`, conflict resolution,
review follow-up, and a follow-up PR for any reviewer knits left open.

**You are the author.** You may edit code, rebase, force-push, and update PR
metadata (draft state). You are **not** merging: cloud agents on this repo do not
have merge permission. When the PR (or entire **GitHub stack**) is green and
reviews are complete, your job is done — the maintainer lands with
`gh stack merge` (see **Merge handoff**).

## GitHub stacks (`gh stack`) — mandatory tooling

Stacked work in this repo is managed with GitHub's **stacked pull requests**
feature (public preview). Install and authenticate before prepare work:

```bash
gh extension install github/gh-stack   # idempotent
gh auth status
```

Official references:

- [Stacked PRs quickstart](https://docs.github.com/en/pull-requests/get-started/stacked-prs-quickstart)
- [Managing stacked pull requests](https://docs.github.com/en/pull-requests/how-tos/create-pull-requests/managing-stacked-pull-requests)
- [Stacked PRs CLI commands](https://docs.github.com/en/pull-requests/reference/stacked-prs-cli-commands)

| Goal | Command |
|------|---------|
| Inspect stack | `gh stack view` / `gh stack view --json` |
| Adopt remote stack locally | `gh stack checkout <pr-number>` |
| Link existing PRs into a GitHub stack (bottom → top order) | `gh stack link <pr#> <pr#> … --open` |
| Push + create/update PRs + stack on GitHub | `gh stack submit --auto --open` |
| Fetch, rebase on trunk, push, sync stack metadata | `gh stack sync` |
| Cascading rebase (conflicts) | `gh stack rebase` / `gh stack rebase --continue` |
| Maintainer merge (agents **do not** run) | `gh stack merge` |

**Do not** retarget stacked PR bases to `master` with `gh pr edit --base master`.
GitHub stacks keep each layer targeting the branch below; landing is a single
stack merge operation, not per-PR retargeting.

## Babysit until green (mandatory — not a one-shot)

**"Prepare for merge" means babysitting the PR until it is actually ready** — not
running a single pass and stopping. Stay on the PR until:

1. **CI / checks** — all required status checks pass (re-run or fix failures;
   retry known flakes once, then report).
2. **Reviews** — no review is still **in progress**. If a reviewer has started a
   review but not submitted (pending review, "review in progress", or equivalent
   on GitHub), **wait** until it is submitted before treating feedback as final.
3. **Feedback** — after reviews land, address all blocking items via `babysit`
   (below) and loop until there are no open blocking threads or check failures.

Poll with `gh pr checks <number>` and `gh pr view <number> --json
reviewDecision,reviews,statusCheckRollup` (or the PR UI). Re-check after each
push. Do not report "ready to merge" while checks are pending/failing or a review
is still in flight.

## Prerequisite: run `babysit` (mandatory for review feedback)

**`babysit` is Cursor's built-in skill** for addressing PR review comments, CI
failures, and other blockers. After rebase/conflict work, **load and follow
`babysit`** to completion on this PR.

**How to run the built-in babysit step:**

1. **Check your environment** for Cursor's `babysit` skill. If it is available
   (slash command, cursor command, or equivalent in your skill catalog), **load
   its instructions into context** before continuing.
2. **Invoke it explicitly** when your environment supports slash commands: run
   `babysit` on the PR or branch. Do not skip invocation and improvise a
   substitute from this file alone.
3. **Confirm in chat** (one line) that `babysit` ran — e.g. "Ran `babysit`;
   proceeding with final status."
4. **Complete the `babysit` workflow** before marking the PR ready to merge.

If `babysit` is **not** available in your environment (e.g. some cloud agents),
say so explicitly in chat before continuing. Apply its intent manually:

1. Read all open review threads and unresolved conversations on the PR.
2. Fix each actionable item with minimal diffs.
3. Run relevant verification gates for this repo (L0 build, L2 crypto tests per
   `AGENTS.md` / `test-plan.md`).
4. Push to the **same PR branch** and reply on resolved threads.

Do **not** skip review follow-up — "prepare for merge" includes clearing blocking
feedback, not only git hygiene.

### Knit follow-up PR (mandatory when knits remain)

Reviewers in this repo may approve with **nits only** (see `pr-review-delivery`
**"Approval policy"**). Those nits are intentionally **not** blockers for merge,
but they still need to be tracked and addressed. When preparing for merge, **open
a follow-up PR** for any unresolved knit feedback.

**What counts as a knit:**

- Review comments tagged `[nit]` (or equivalent nit severity in the thread).
- Items called out in a top-level **"Approve — nits only"** summary that were not
  fixed in the merge PR.
- Optional polish the reviewer explicitly deferred (style, naming, minor cleanup)
  with no behavioral impact.

**What is not a knit** (must be fixed on the merge PR via `babysit`, not deferred):

- `blocking`, `important`, or `question` threads.
- Correctness, safety, ABI, or test-gap issues.
- Any feedback that caused **request changes** or prevented approval.

**Follow-up workflow:**

1. **Inventory knits** — after `babysit`, list every open knit thread or deferred
   nit from the latest review. If there are none, skip this section.
2. **Branch** — from the prepared merge PR head (post-rebase):

   ```bash
   git checkout <head-branch>
   git checkout -b cursor/<descriptive-knit-follow-up>-f18e
   ```

3. **Fix knits** — minimal diffs only; do not expand scope beyond the listed nits.
4. **Verify** — run applicable gates from `AGENTS.md` for the knit fixes.
5. **Push and open PR** — push the branch and create a PR targeting `master` via
   `ManagePullRequest` `create_pr`. In the body:
   - **`@toadslop`** — maintainer notification (required; near the top).
   - Link the parent PR (e.g. "Follow-up to #N — addresses reviewer nits").
   - List each knit addressed (with thread links or short quotes).
   - Note that it should merge **after** the parent PR lands (or rebase onto
     `master` once the parent is merged if CI requires a clean base).
6. **Do not fold knits into the merge PR** when the reviewer approved with nits
   only — keep the merge PR focused; the follow-up carries the polish.

If knits are ambiguous (nit vs important), treat them as blocking and fix them on
the merge PR via `babysit` instead of deferring.

## Stack readiness gate (mandatory — run first)

**Do nothing destructive** (no `gh stack rebase`, force-push, or `gh stack modify`)
until this gate passes.

### 1. Identify the PR

Resolve the target PR by number, URL, or current branch:

```bash
gh pr view --json number,title,state,isDraft,baseRefName,headRefName,url
# or: gh pr view <number-or-branch> --json ...
```

| Outcome | Action |
|---------|--------|
| No PR for current branch | Stop — ask the user which PR to prepare. |
| `state` is `MERGED` | Stop — report the PR is already merged; nothing to prepare. |
| `state` is `CLOSED` (not merged) | Stop — ask whether to reopen or use a different PR. |
| `isDraft` is `true` | Mark ready for review (see **Draft → open** below), then continue. |

Record: `PR`, `head` = `headRefName`, `base` = `baseRefName`.

### Draft → open

If the PR is a **draft**, mark it **ready for review** before retarget/rebase work
(draft PRs often skip or delay required checks and reviews):

```bash
gh pr ready <number>
```

Use `ManagePullRequest` `update_pr` with draft-appropriate fields when available.
Confirm with `gh pr view <number> --json isDraft` that `isDraft` is `false`.

### 2. Discover stack membership (bottom → top)

Build the **local stack chain** for this PR (same issue or dependency stack):

1. Start from the target PR's `headRefName`.
2. Walk **up** the stack: find open PRs whose `baseRefName` equals the current
   layer's `headRefName` (there should be at most one).
3. Walk **down** from the target PR: follow `baseRefName` while another open PR's
   `headRefName` matches that base.

Record ordered PR numbers **`[#bottom, …, #top]`** and branches. The **bottom**
layer is the only one that may target `master` (or a dependency branch already
integrated into `master`).

```bash
# After you know PR numbers bottom → top:
gh stack link <bottom-pr#> ... <top-pr#> --open   # creates/updates GitHub stack
gh stack checkout <bottom-pr#>                    # local tracking + branches
gh stack view
```

If PRs are already linked on GitHub, `gh stack checkout <any-pr-in-stack>` is
enough — skip `link` unless composition changed.

| Chain size | Prepare mode |
|------------|----------------|
| **1 PR**, `base` = `master` | **Single PR** — manual rebase onto `master` (below) |
| **2+ PRs** in a chain | **GitHub stack** — `gh stack sync` / `gh stack rebase`; never retarget bases to `master` |
| **1 PR**, `base` ≠ `master` | See **Bottom-layer gate** — usually blocked until parent lands |

### 3. Bottom-layer gate (mandatory)

Only the **bottom** PR of a chain must sit on an integrated trunk:

1. If the bottom PR's `base` is `master` → gate **passed** for the whole stack.
2. If the bottom targets branch `base` (dependency PR), resolve that PR:

   ```bash
   gh pr view "$base" --json number,state,mergedAt,url 2>/dev/null || true
   ```

3. **Parent merged (primary).** `state: MERGED` or `mergedAt` set → gate **passed**.
4. **Git ancestry (supplementary).** When no merged PR record exists:

   ```bash
   git fetch origin master "$base" --prune
   git merge-base --is-ancestor "origin/$base" origin/master
   ```

5. **If neither passes** → **STOP.** Do not sync, rebase, or push. Example message:

   > Stack bottom is not on `master` yet (stacked on `<base>` / PR #N). Land the
   > dependency first, then prepare the stack again.

**Upper layers** (`baseRefName` = previous PR branch) are expected while the stack
is open — they are **not** blocked by an "unmerged parent" in the GitHub stack
model. Prepare the **entire chain** when the bottom gate passes.

If you were asked to prepare a **middle/top** PR alone, still run prepare on the
**full stack** from the bottom PR number.

### 4. Confirm with the user (when ambiguous)

If stack topology is unclear (forked chains, duplicate heads, or base was
force-pushed), stop and ask before `gh stack rebase` / `gh stack modify`.

## Prepare workflow

Run only after the **Stack readiness gate** passes.

### 1. Sync local `master`

```bash
git fetch origin master
```

### 2a. GitHub stack (2+ chained PRs)

```bash
gh stack checkout <bottom-pr-number>
gh stack link <bottom-pr#> ... <top-pr#> --open   # if not already linked
gh stack sync
```

- If `sync` aborts on divergence (non-interactive agent), diagnose with
  `gh stack view --json`, align local/remote composition, or use
  `gh stack rebase` after fetching `master`.
- On rebase conflicts: resolve, `git add`, then `gh stack rebase --continue`
  (or `gh stack sync` again when appropriate).
- Confirm bases still chain (bottom → `master`, each upper layer → branch below).
  **Never** `gh pr edit --base master` on stack layers.

### 2b. Single PR on `master` (chain size 1)

```bash
git checkout <head-branch>
git rebase origin/master
git push --force-with-lease origin <head-branch>
```

On conflicts: resolve, `git add`, `git rebase --continue`, then push.

### 3. Mark drafts ready (whole stack)

For **every** PR in the chain (or the single PR) still in draft:

```bash
gh pr ready <number>
```

Use `ManagePullRequest` `update_pr` when available. Confirm `isDraft` is `false`
on all stack layers before babysitting.

### 4. Babysit until green (`babysit` + checks + reviews)

Follow **"Babysit until green"** above. Run Cursor's built-in **`babysit`** skill
on **each layer** in the stack (or the single PR) before handoff — see
**"Prerequisite: run `babysit`"**. For each layer it should:

- **Wait** for any in-progress review to finish before responding.
- Resolve open review comments and requested changes (per `babysit` / manual
  fallback rules in this file).
- Fix CI failures tied to the branch; poll until `gh pr checks` is green.
- Re-run verification gates after each fix pass.
- Push to the **same head branch** (no new PR).

**Loop** until: all required checks pass, no review is in progress, and there are
no blocking review items (or the user accepts known flakes).

### 5. Open knit follow-up PR (when applicable)

After `babysit` clears blocking feedback, run the **"Knit follow-up PR"** workflow
above if any reviewer knits remain open. This step is part of "ready to merge" —
do not skip it when nits were left unresolved at approval time.

### 6. Merge handoff (agents stop here)

When every layer in the stack (or the single PR) passes the babysit loop:

- Report **`ready for maintainer merge`**.
- Include `gh stack view --short` output (or stack number from the GitHub UI).
- Tell the maintainer they can land with **`gh stack merge`** (interactive) or
  `gh stack merge <top-pr#> --yes --squash` when appropriate for this repo.

**Do not run `gh stack merge`** yourself — agents lack merge permission and must
not bypass branch protection.

### 7. Final status report

Reply in chat with:

| Item | Status |
|------|--------|
| PR(s) / stack | numbers bottom → top; GitHub stack linked? |
| Bottom base | should be `master` (or integrated dependency branch) |
| `gh stack sync` / rebase | yes / conflicts resolved (note) |
| Conflicts | none / resolved (brief note) |
| Draft → open | all layers / n/a |
| CI / checks | all green per layer / pending / failing |
| Review feedback | addressed via `babysit` / waiting / remaining |
| Reviewer knits | none / follow-up PR link |
| Ready for `gh stack merge` | yes / no — and why |

**Do not merge** unless the user explicitly asks and your environment has permission.

## Boundaries

| Do | Do not |
|----|--------|
| Mark draft PRs ready for review (`gh pr ready`) | Leave a draft PR in draft state while "preparing" |
| Babysit until CI is green and reviews are complete on **every stack layer** | Stop after one fix pass while checks fail or a review is in progress |
| `gh stack link` + `gh stack sync` / `gh stack rebase` for chains | `gh pr edit --base master` on stacked layers |
| `gh stack push` / sync force-with-lease when needed | Run `gh stack merge` (maintainer-only) |
| Fix conflicts and review feedback | Run the stack gate after destructive git ops |
| Stop when stack **bottom** is not on integrated trunk | Prepare only the top layer while bottom is blocked |
| Use `babysit` for review/CI follow-up | Post new review findings (reviewer role) |
| Open a follow-up PR for unresolved reviewer knits | Fold deferred nits into the merge PR |
| Report **ready for `gh stack merge`** when green | Claim the stack was merged |

## Relationship to other skills

| Skill | Role |
|-------|------|
| **`prepare-all-prs-for-merge`** | Batch wrapper — runs this skill on every eligible open PR. |
| **`babysit`** (Cursor built-in) | Address review comments, CI, and PR hygiene (invoked during prepare). |
| **`pr-review-delivery`** | Reviewer-only — do **not** use when preparing for merge. |
| **`code-review`** (Cursor built-in) | Reviewer analysis — out of scope for this author workflow. |

## Repo verification (this project)

After code changes from conflict resolution or review fixes, run applicable gates
from `AGENTS.md`:

- **L0** — `make clean && make KDIR=/opt/linux LLVM=1 -j"$(nproc)"` (with
  `LIBCLANG_PATH=/usr/lib/llvm-18/lib`)
- **L2** — `make -C tests/host/crypto all` when crypto code changed
- **L1** — `make rust-check-symbols` after C→Rust object swaps

Skip gates that do not apply to the diff; run any gate touched by your changes.
