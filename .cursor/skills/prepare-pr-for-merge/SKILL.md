---
name: prepare-pr-for-merge
description: >-
  Prepares a pull request for merge into master. Auto-applies on "prepare PR for
  merge", "prepare for merge", "get PR ready to merge", or similar author tasks.
  Validates that stacked-PR ancestors are already merged; retargets stacked PRs to
  master; rebases on master; resolves conflicts; and addresses review feedback via
  Cursor's built-in babysit skill. Do NOT use for reviewing PRs (use
  pr-review-delivery) or for PRs whose stack base is not yet on master.
metadata:
  requires-skill: babysit
---

# Prepare PR for Merge

Use this skill when the user (or you as the PR author) wants a pull request **ready
to land on `master`** — not merely "address comments", but the full pre-merge
prep: stack validation, base retarget, rebase, conflict resolution, and review
follow-up.

**You are the author.** You may edit code, rebase, force-push, and update PR
metadata (base branch). You are **not** merging the PR unless the user explicitly
asks you to merge.

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

## Stack readiness gate (mandatory — run first)

**Do nothing destructive** (no base change, rebase, or force-push) until this gate
passes.

### 1. Identify the PR

Resolve the target PR by number, URL, or current branch:

```bash
gh pr view --json number,title,state,baseRefName,headRefName,url
# or: gh pr view <number-or-branch> --json ...
```

| Outcome | Action |
|---------|--------|
| No PR for current branch | Stop — ask the user which PR to prepare. |
| `state` is `MERGED` | Stop — report the PR is already merged; nothing to prepare. |
| `state` is `CLOSED` (not merged) | Stop — ask whether to reopen or use a different PR. |

Record: `PR`, `head` = `headRefName`, `base` = `baseRefName`.

### 2. Verify the stack parent is on `master`

A **stacked PR** targets another feature branch instead of `master`
(`baseRefName` ≠ `master`). Before preparing it for merge, that **parent branch**
must already be integrated into `master`.

1. If `base` is `master` → stack gate **passed**; continue to **Prepare workflow**.
2. Otherwise the PR is stacked on branch `base`. Resolve the PR for `base`, if any:

   ```bash
   gh pr view "$base" --json number,state,mergedAt,url 2>/dev/null || true
   ```

3. **Parent PR merged (primary check).** If the PR for `base` has `state: MERGED`
   (or `mergedAt` is set) → stack gate **passed**. This repo squash-merges PRs,
   so the branch tip is often **not** a git ancestor of `origin/master` even after
   merge — do not rely on ancestry alone.

4. **Git ancestry (supplementary).** If there is no merged PR for `base`, fetch and
   test whether `base` is integrated into `master`:

   ```bash
   git fetch origin master "$base" --prune
   # Exit 0 = integrated; exit 1 = not integrated:
   git merge-base --is-ancestor "origin/$base" origin/master
   ```

   Use this when the parent landed via a merge commit or branch was fast-forwarded
   into `master` without a squash-merge PR record.

5. **If neither check passes** → **STOP immediately.** Do not change base, rebase,
   or push. Report clearly, for example:

   > This PR is **not ready** to prepare for merge. It is stacked on
   > `<base>` (PR #N), which is not merged into `master` yet. Merge or land
   > that earlier PR first, then run prepare-for-merge again.

   Include the blocking PR link/number and what the user should do next.

6. **If either check passes** → stack gate **passed**. The prepare step will
   retarget this PR's base from `base` to `master` and rebase onto `master`.

Do not retarget or rebase while the direct parent PR is still open and `base` is
not integrated into `origin/master`.

### 3. Confirm with the user (when ambiguous)

If stack topology is unclear (multiple open PRs share branch names, or base was
force-pushed), stop and ask before rewriting history.

## Prepare workflow

Run only after the **Stack readiness gate** passes.

### 1. Sync local `master`

```bash
git fetch origin master
git checkout master && git pull origin master
```

### 2. Retarget stacked PRs to `master`

If `baseRefName` ≠ `master`, update the PR base **to `master` only** — do not
retarget onto another feature branch:

```bash
gh pr edit <number> --base master
```

Use `ManagePullRequest` `update_pr` with `base_branch: master` when available.

Confirm with `gh pr view <number> --json baseRefName` that base is `master`.

### 3. Rebase the PR branch onto `master`

```bash
git checkout <head-branch>
git rebase origin/master
```

- On conflicts: resolve, `git add` the resolved files, `git rebase --continue`.
- If the rebase is wrong or too messy: `git rebase --abort` and reassess; prefer
  a clean commit history over rushed conflict markers.
- After a successful rebase: `git push --force-with-lease origin <head-branch>`.

### 4. Address review feedback (`babysit`)

Run Cursor's built-in **`babysit`** skill on this PR (see **"Prerequisite: run
`babysit`"** above). It should:

- Resolve open review comments and requested changes.
- Fix CI failures tied to the branch.
- Re-run verification gates after each fix pass.
- Push to the **same head branch** (no new PR).

Loop until there are no blocking review items and CI is green (or the user
accepts known flakes).

### 5. Final status report

Reply in chat with:

| Item | Status |
|------|--------|
| PR link / number | |
| Base branch | should be `master` |
| Rebased onto latest `master` | yes / no |
| Conflicts | none / resolved (brief note) |
| Review feedback | addressed via `babysit` / remaining items |
| Ready to merge | yes / no — and why |

**Do not merge** unless the user explicitly asks.

## Boundaries

| Do | Do not |
|----|--------|
| Retarget stacked PR base to `master` | Retarget onto another feature branch |
| Rebase and force-push with `--force-with-lease` | Merge the PR without explicit instruction |
| Fix conflicts and review feedback | Run the stack gate after destructive git ops |
| Stop and report when stack parent is unmerged | Rebase a PR blocked by an open ancestor |
| Use `babysit` for review/CI follow-up | Post new review findings (reviewer role) |

## Relationship to other skills

| Skill | Role |
|-------|------|
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
