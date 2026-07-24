---
name: pr-review-delivery
description: >-
  Repo-specific addendum for pull request reviews in this RTL88x2BU driver project.
  Use alongside Cursor's built-in /review (or /review-bugbot, /review-security) —
  not as a replacement. Adds mandatory PR comment posting and migration-specific
  checks. Invoke explicitly with /pr-review-delivery or @pr-review-delivery after
  or during a /review run.
disable-model-invocation: true
---

# PR Review Delivery (repo addendum)

This skill **complements** Cursor's built-in review skills. It does **not**
replace them.

| Built-in skill | Role |
|----------------|------|
| `/review` | Primary code review — selects and runs the appropriate review agent |
| `/review-bugbot` | Bug and regression focus |
| `/review-security` | Security vulnerability focus |

Use those skills (especially `/review`) for analysis, reasoning, and finding
issues. Use **this skill** for two repo-specific additions:

1. **Delivery** — publish every actionable finding on the pull request itself.
2. **Migration context** — extra checks for this C→Rust kernel-module migration.

## Workflow

1. **Run the built-in review** — invoke `/review` (or a specialized variant) on
   the PR or branch. Let that skill drive the core review process.
2. **Apply repo-specific checks** — walk the "Migration focus" section below for
   anything the general review may not cover.
3. **Post all findings on the PR** — follow "Post on the PR" below. This includes
   findings from `/review` **and** any migration-specific items you found.
4. **Recap in chat** — brief pointer to the PR; do not duplicate the full review.

If the user only invokes this skill (not `/review`), still perform a thorough
review — but treat Cursor's `/review` as the preferred primary workflow and
mention that in your summary.

## Post on the PR (mandatory)

When a pull request exists, you **must** publish feedback on that PR before
finishing. Chat output is a pointer to what you posted, not a substitute.

1. **Identify the PR** — use `branch_name`, a PR number/URL from the user, or
   `gh pr list` / `gh pr view` if needed.
2. **Post review comments** — use `ManagePullRequest` with `action: post_comment`.
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

Apply these checks in addition to whatever `/review` surfaces:

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
