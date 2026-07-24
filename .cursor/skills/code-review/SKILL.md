---
name: code-review
description: >-
  Review pull requests for this RTL88x2BU driver repo. Use when the user asks for
  a code review, PR review, or feedback on a branch/PR — including "review PR #N",
  "look at this diff", or re-review after follow-up commits.
---

# Code Review

Review changes for correctness, safety, and alignment with the C→Rust migration
plan. **Feedback must land on the pull request itself** so authors can see,
discuss, and act on it — not only in the agent chat.

## Non-negotiable: post on the PR

When a pull request exists for the branch under review, you **must** publish your
feedback on that PR before finishing. Chat output is a brief pointer to what you
posted, not a substitute for PR comments.

1. **Identify the PR** — use `branch_name`, a PR number/URL from the user, or
   `gh pr list` / `gh pr view` if needed.
2. **Post review comments** — use `ManagePullRequest` with `action: post_comment`.
3. **Confirm delivery** — your final message should link to or name the PR and
   state how many inline vs summary comments you posted.

If no PR exists yet, say so in chat and ask whether to open one or review the
diff only. Do not skip posting just because the review is short.

### How to post comments

Use `ManagePullRequest` (`post_comment`). Prefer **inline review comments** on
the changed lines; add one **top-level summary** when useful.

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

## Review workflow

1. **Scope** — read the PR description, linked issue/epic, and the full diff.
   Note which migration wave/issue the PR targets (`docs/rust-migration/issues/`).
2. **Build context** — for Rust/C object swaps, check `docs/rust-migration/test-plan.md`
   for applicable gates (L0 build, L1 symbols, L2 host tests, L3 QEMU load).
3. **Review the diff** — correctness, memory safety, ABI stability, error paths,
   test coverage, Makefile/kbuild wiring, and docs.
4. **Post findings on the PR** — one inline comment per distinct issue; group nits
   only when they share the same root cause.
5. **Post a summary comment** — verdict (`approve` / `request changes` / `comment`),
   blocking items, and what was verified (e.g. "L2 harness passes").
6. **Reply in chat** — short recap with PR link; do not paste the full review.

## Re-reviews

When reviewing after the author pushed fixes:

- Reply on existing threads with `in_reply_to` when addressing prior comments.
- Post a new top-level summary noting which earlier items are resolved and what
  remains open.
- Do not re-post the same inline comment on an unchanged line.

## Repo-specific focus

Prioritize checks relevant to this kernel-module migration:

- **ABI / linkage** — exported symbols unchanged after C→Rust swaps (L1:
  `make rust-check-symbols`).
- **FFI boundaries** — `unsafe` blocks minimal; pointer/length validation at the
  C/Rust edge; no panics across FFI.
- **Crypto correctness** — host differential tests (`make -C tests/host/crypto test`)
  for touched algorithms; vectors cover edge cases.
- **Kbuild** — `Makefile` object lists, `RUST` targets, and compile-only gates
  stay consistent; no dead Makefile symbols.
- **Kernel constraints** — no host-only assumptions in module code; no `insmod` on
  the cloud host (use QEMU per `AGENTS.md` for L3).

## Tone

Be direct and specific. Every comment should be actionable. Praise good patterns
briefly when they reduce future migration risk.
