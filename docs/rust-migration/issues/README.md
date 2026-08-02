# Rust migration issues

**GitHub Issues are the source of truth** for what exists, what is open or closed,
and what blocks what. Do not maintain a parallel status table in the repo.

- **Tracker:** https://github.com/toadslop/rtl88x2bu-linux-driver/issues
  (filter: `label:rust-migration`)
- **Draft specs:** `docs/rust-migration/issues/*.md` — templates for **new** work;
  once filed, the live issue body on GitHub is authoritative
- **Filing registry:** [`ISSUE-MAP.md`](ISSUE-MAP.md) — draft ID ↔ GitHub `#N`,
  updated only by [`file-issues.sh`](file-issues.sh) (not status)

Landed implementation docs (not issue tracking):

- [`docs/rust-migration.md`](../../rust-migration.md) — phases, ABI, build contract, PR checklist
- [`docs/smoke-test.md`](../../smoke-test.md) — L4 hardware STA checklist
- [`../architecture.md`](../architecture.md) — domain types / layering
- [`../test-plan.md`](../test-plan.md) — L0–L4 gates
- [`../dev-environment.md`](../dev-environment.md) — toolchain / pinned kernel / L3 QEMU gotchas (Wave 0)

## Querying the tracker

```bash
# Open migration issues
gh issue list --label rust-migration --state open --limit 200

# Issue details (state, Tracking footer with blocked_by)
gh issue view <number> --json number,title,state,body,labels

# Map draft ID → GitHub number (filing registry)
grep 'W3-19' docs/rust-migration/issues/ISSUE-MAP.md
```

**Ready frontier:** open children whose `blocked_by` dependencies are all
**satisfied** — closed on GitHub **or** accessible via an open implementing PR
(read each issue's `## Tracking` footer). Prefer selecting from **parallel
lanes** (different C files) when multiple issues are ready. Use `gh issue list` /
`gh issue view` — not local markdown — to decide what is ready.

**Epic planning:** `epic-*.md` files describe wave structure and deferred scope.
They are planning notes, not a substitute for GitHub issue state.

## Filing new issues

Draft specs use YAML frontmatter (`id`, `epic`, `blocked_by`, `labels`) plus
Goal / Notes / Acceptance sections. Use existing children as templates (e.g.
`wave3-04-security-type-str.md`).

### `blocked_by` — favor parallel work

This port spans many independent `core/` translation units. **Default to wide
graphs:** most new issues should be immediately workable in parallel (`blocked_by:
[]` or infra-only deps like `T5` / `A2`).

Use `blocked_by` **only** for real technical coupling:

- Same C file, later slice needs Rust from an earlier slice
- Missing harness or domain-type infra
- True cross-file API dependency

Do **not** chain issues just because IDs are sequential or because they appear in
the same epic tranche table. Different C files (`rtw_rf.c`, `rtw_recv.c`,
`rtw_mlme.c`, …) are normally **independent lanes**. See
[`.cursor/skills/draft-migration-issues/SKILL.md`](../../../.cursor/skills/draft-migration-issues/SKILL.md)
(**Favor wide graphs over deep chains**).

From the repo root:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

The script:

- Creates labels (if needed), opens issues, and appends rows to `ISSUE-MAP.md`
- Is **idempotent** — skips draft IDs already in `ISSUE-MAP.md` (or matching titles)
- Appends map rows after the last table row (preserves filing order)
- Appends a `## Tracking` footer with resolved `#N` links for `epic` / `blocked_by`
- On re-run, refreshes **only** the `## Tracking` section when it still contains
  `(not filed yet)` (use `FORCE_REFRESH=1` to rewrite Tracking anyway)
- Rewrites relative markdown links to GitHub blob URLs (override ref with
  `FILE_ISSUES_REF=<sha-or-branch>`)

After filing, verify on GitHub — do not add status rows to this README.

**GitHub-only IDs:** W3-10…W3-18 (#179…#187) were filed before local draft specs
existed. They are in `ISSUE-MAP.md` but not in `file-issues.sh`'s `files[]` list;
use `gh issue edit` to refresh their Tracking footers until draft markdown is added.

## Sizing

Each implementable child targets about **~200 lines** of meaningful change
(roughly 150–250). Do not bundle unrelated work.

## Verification (required)

Hardware is **not** the default gate. See [`../test-plan.md`](../test-plan.md)
and [`../architecture.md`](../architecture.md):

- **Characterize C behavior → freeze Rust tests → port** (parity first)
- **L0 build** + **L1 symbols** on every C→Rust swap
- **L2 host/unit differential tests** for crypto/pure chunks
- **Domain types** at Rust APIs; raw pointers only in abi/os shims
- **L3 VM insmod** when touching init; **L4 hardware** at wave milestones

Test-infra epics/issues: label `rust-migration` + `wave-*` / `phase-*` on GitHub.
See open issues tagged `phase-1` / `wave-0` … `wave-6` for current work.
