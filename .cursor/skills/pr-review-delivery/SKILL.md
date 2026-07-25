---
name: pr-review-delivery
description: >-
  Delivery addendum for PR reviews in this RTL88x2BU driver repo. Auto-applies on
  reviewer tasks ("review PR", "code review", "look at this diff", re-reviews) but
  only AFTER Cursor's native /code-review command is loaded and run — never as a
  substitute. Adds mandatory PR comment posting and migration-specific checks on
  top of /code-review output. Do NOT use for "address review comments", "respond
  to review", or "fix PR feedback" (author tasks). Reviewers must not change PR
  state (close, draft, ready-for-review, merge, etc.) — comments only.
metadata:
  requires-skill: code-review
  layer: delivery-addendum
---

# PR Review Delivery (repo addendum)

This skill **auto-applies** when you are **reviewing** someone else's pull request
— checking whether the code is correct and posting findings on the PR.

## Prerequisite: run `/code-review` first (mandatory)

**This skill is a delivery and migration addendum. It does not perform core code
review.** Before following any other section of this file, you **must** load
Cursor's native `/code-review` command into context and complete its analysis
workflow.

`/code-review` is Cursor's built-in code review command. It instructs the agent
to review with a code-review mindset: prioritize bugs, behavioral regressions,
security issues, and missing tests; order findings by severity; and do not make
code changes unless explicitly asked.

**How to run the native code review step:**

1. **Check your environment** for Cursor's `/code-review` command. If it is
   available (slash command, cursor command, or equivalent in your skill catalog),
   **load its instructions into context before continuing** — treat it as step
   zero of this workflow.
2. **Invoke it explicitly** when your environment supports slash commands: run
   `/code-review` on the PR or branch. Do not skip invocation and improvise a
   substitute review from this file alone.
3. **Confirm in chat** (one line) that `/code-review` ran — e.g. "Ran
   `/code-review`; proceeding with delivery addendum."
4. **Complete the `/code-review` analysis** before applying migration checks or
   posting PR comments below.

**Do not proceed** to "Migration focus" or "Post on the PR" until `/code-review`
is done. Findings you post must include everything from `/code-review` **plus**
any migration-specific items from this skill.

If `/code-review` is **not** available in your environment (e.g. some cloud
agents), say so explicitly in chat before continuing. You may still apply this
delivery addendum, but do not pretend `/code-review` ran — note the gap.

**You are the reviewer, not the author.** Do not write code, push commits, rebase
the branch, merge, or otherwise act as the PR author. Your only job is to review
the diff and post comments.

**Do not change PR state.** Never close, reopen, mark as draft, mark ready for
review, merge, or otherwise change the PR's lifecycle — even if you think it is
superseded, redundant, or ready to land. State changes are the author's (or
maintainer's) job, not yours.

**This skill does not apply when you are the author addressing review feedback.**
For "address review comments", "respond to review", or "fix PR feedback": read the
PR comments, make the fixes, commit, and **push to the same PR branch** (do not
create a new branch or a new PR unless the user explicitly asks).

This skill **complements** Cursor's native `/code-review` command — it does **not**
replace or override it. See **"Prerequisite: run `/code-review` first"** above.

On top of whatever `/code-review` produces, this skill adds:

1. **Delivery** — publish every actionable finding on the pull request itself.
2. **Migration context** — extra checks for this C→Rust kernel-module migration.

## Author workflow (out of scope for this skill)

When the user asks you to **address**, **respond to**, or **fix** review
comments on a PR, you are the **author**, not the reviewer. Do **not** invoke
this skill or post new review findings. Instead:

1. **Find the PR** — by number, URL, or branch name (`gh pr view`, `gh api` for
   inline review threads).
2. **Read all open review comments** — inline threads and top-level summaries.
3. **Fix the code** — minimal diff that resolves each actionable comment.
4. **Verify** — run the relevant gates (L0/L2 per `AGENTS.md` and `test-plan.md`).
5. **Push to the same PR branch** — `git push origin <head-branch>`; do **not**
   create a new branch or a new PR unless the user explicitly asks.
6. **Recap in chat** — list which comments were addressed and what you verified.

Optionally reply on resolved threads with `ManagePullRequest` `post_comment` and
`in_reply_to` to note the fix — that is author follow-up, not a new review.

## Workflow (reviewer only)

**Reviewer boundaries — do only this:**

| Do | Do not |
|----|--------|
| Read the PR diff and related context | Edit code or push commits to the PR branch |
| Post inline and top-level review comments | Close, reopen, or merge the PR |
| Reply on existing review threads | Mark draft / ready-for-review |
| Summarize findings in chat (brief pointer to PR) | Rebase, force-push, or create a new PR |
| Run read-only verification to inform comments | Change PR title, body, labels, or reviewers |

1. **Run `/code-review`** — follow **"Prerequisite: run `/code-review` first"**
   above. This step is non-negotiable; do not skip to delivery or migration
   checks without it.
2. **Apply repo-specific checks** — walk the "Migration focus" section below for
   anything the general review may not cover.
3. **Post all findings on the PR** — follow "Post on the PR" below. This includes
   findings from `/code-review` **and** any migration-specific items you found.
4. **Recap in chat** — brief pointer to the PR; do not duplicate the full review.

When this skill auto-applies, you **still** run `/code-review` first — this skill
is the delivery and migration layer, not a substitute for Cursor's code review.

## Post on the PR (mandatory)

When a pull request exists, you **must** publish feedback on that PR before
finishing. Chat output is a pointer to what you posted, not a substitute.

1. **Identify the PR** — use `branch_name`, a PR number/URL from the user, or
   `gh pr list` / `gh pr view` if needed.
2. **Post review comments** — use `ManagePullRequest` with `action: post_comment`
   only. Do **not** use `set_pr_status`, `create_pr`, `update_pr`, or any action
   that changes PR state, metadata, or branch contents.
3. **Confirm delivery** — your final message should link to or name the PR and
   state how many inline vs summary comments you posted.

If no PR exists yet, say so in chat and ask whether to open one or review the
diff only.

### How to post comments

| Feedback type | `post_comment` shape |
|---------------|----------------------|
| Issue on a specific line | `path`, `line`, `body` (and `side: RIGHT` for new code) |
| Issue spanning a few lines | `path`, `start_line`, `line`, `body` |
| Whole-file or architectural note | `path`, `body` (no `line`) |
| Overall verdict / re-review status | `body` only (top-level PR comment) |

**Inline comment body format:**

```markdown
**[severity] Short title**

What's wrong and why it matters.

Suggested fix (if applicable):
```

Severity: `blocking`, `important`, `nit`, or `question`.

**Do not** use `gh pr comment` or `gh api` for review feedback when
`ManagePullRequest` is available.

## Re-reviews

When reviewing after the author pushed fixes:

- Reply on existing threads with `in_reply_to` when addressing prior comments.
- Post a new top-level summary noting which earlier items are resolved and what
  remains open.
- Do not re-post the same inline comment on an unchanged line.

## Migration focus (repo-specific)

Apply these checks in addition to whatever `/code-review` surfaces:

- **ABI / linkage** — exported symbols unchanged after C→Rust swaps (L1:
  `make rust-check-symbols`). See `docs/rust-migration/test-plan.md`.
- **FFI boundaries** — `unsafe` blocks minimal; pointer/length validation at the
  C/Rust edge; no panics across FFI.
- **Crypto correctness** — host differential tests (`make -C tests/host/crypto test`)
  for touched algorithms; vectors cover edge cases.
- **Kbuild** — `Makefile` object lists, `RUST` targets, and compile-only gates
  stay consistent; no dead Makefile symbols.
- **Kernel constraints** — no host-only assumptions in module code; no `insmod` on
  the cloud host (use QEMU per `AGENTS.md` for L3).
- **Issue linkage** — note which migration wave/issue the PR targets
  (`docs/rust-migration/issues/`).

## Tone

Be direct and specific. Every comment should be actionable.
