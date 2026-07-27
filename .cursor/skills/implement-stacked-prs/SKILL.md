---
name: implement-stacked-prs
description: >-
  Step 3b of pick-up-work-item. Implements an approved stacked-PR plan one PR
  at a time: branch, code, gates, push, open PR with stack base. Auto-applies
  after plan-stacked-prs approval. Do NOT use without an approved plan or for
  PRs unrelated to the selected issue.
metadata:
  parent-skill: pick-up-work-item
  step: 3b
---

# Implement Stacked PRs

Execute the approved plan from **`plan-stacked-prs`** — one PR at a time, bottom
of the stack first.

## Before starting

- [ ] Plan approved (explicit user OK or "go ahead and implement")
- [ ] Issue selected and spec read
- [ ] `master` fetched and up to date
- [ ] No open PR already covers PR1 of this stack (avoid duplicates)

```bash
git fetch origin master
git checkout master && git pull origin master
```

## Per-PR loop

Repeat for each row in the plan table (PR1 → PR2 → …):

### 1. Branch

```bash
# PR1 — base master
git checkout -b cursor/<name>-<suffix> origin/master

# PR2+ — base previous PR branch
git fetch origin cursor/<prev-branch>
git checkout -b cursor/<name>-<suffix> origin/cursor/<prev-branch>
```

Cloud agents: branch names must match `cursor/<descriptive-name>-e465` when that
suffix is configured for the run.

### 2. Implement

- Follow the draft spec and per-PR plan scope
- Match existing code style in `rust/` and C shims
- **Characterize C → freeze tests → port** (per `architecture.md`)
- Minimal diff — no drive-by refactors

### 3. Verify gates

Run applicable gates from [`AGENTS.md`](../../AGENTS.md):

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
| **L3** | Init / load path changes — QEMU recipe in `dev-environment.md` |

Do not open a PR with failing gates for its scope.

### 4. Commit and push

```bash
git add -A
git commit -m "<type>: <short description> (#<issue>)"
git push -u origin HEAD
```

Reference the GitHub issue in the commit message (`#115`, `W3-04`).

### 5. Open stacked PR

Use `ManagePullRequest` `create_pr`:

| Field | PR1 | PR2+ |
|-------|-----|------|
| `base_branch` | `master` | previous PR head branch |
| `branch_name` | current head | current head |
| `title` | from plan | from plan |
| `body` | link issue, gates run, stack position | + "Stacked on #N" |

PR body should include:

- `Closes #N` or `Part of #N` (use **Closes** only on the final PR of the stack)
- Gates executed
- Stack diagram or "PR 2 of 3 — base: `cursor/...`"

### 6. Update tracking

- Add `In-flight: <branch>` to the issue via comment if not already noted
- Do not close the issue until the **last** PR merges and acceptance is met

### 7. Continue or pause

- **Default:** implement the next PR in the stack in the same session if gates pass
- **Pause** after opening a PR if the user asked for incremental delivery
- After the **final** PR opens, summarize the full stack with links

## Stack hygiene

| Rule | Why |
|------|-----|
| Each PR targets its planned base branch | preserves reviewable increments |
| Do not retarget bases until `prepare-pr-for-merge` | that skill owns landing on `master` |
| Rebase only when necessary to fix conflicts | prefer adding a fix-up commit on the stack |
| Keep PRs ~200 LOC | easier review, matches issue sizing |

## When implementation fails

| Situation | Action |
|-----------|--------|
| Scope bigger than planned | Stop, revise plan (return to `plan-stacked-prs`), do not cram |
| Blocked by missing harness | Implement harness PR first or switch to `draft-migration-issues` for a new `T*` ticket |
| Gate fails | Fix on the same branch before opening PR |
| Dependency issue still open | Stop stack; return to `select-ready-issue` |

## Completion report

```markdown
**Issue:** W3-04 / #115

**Stack opened:**
| PR | Branch | Base | Status |
|----|--------|------|--------|
| #200 | cursor/w3-04a-… | master | open |
| #201 | cursor/w3-04b-… | cursor/w3-04a-… | open |

**Gates:** L0/L1/L2 green on PR2

**Next:** review stack; run `prepare-pr-for-merge` on PR1 when ready to land
```
