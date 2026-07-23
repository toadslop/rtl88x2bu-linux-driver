# Rust migration issue tracker

GitHub Issues are currently **disabled** on this repository, so work items live here until they can be filed.

Landed docs (implementation, not GitHub Issues):

- [`docs/rust-migration.md`](../../rust-migration.md) — phases, ABI, build contract, PR checklist
- [`docs/smoke-test.md`](../../smoke-test.md) — L4 hardware STA checklist
- [`../architecture.md`](../architecture.md) — domain types / layering
- [`../test-plan.md`](../test-plan.md) — L0–L4 gates

## How to use

1. Enable Issues: repo **Settings → General → Features → Issues**.
2. From the repo root, run:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

That script creates labels (if needed), opens epic issues, then child issues, and maintains `ISSUE-MAP.md` with number ↔ draft-id mappings.

Properties:

- **Idempotent** — skips draft IDs already in `ISSUE-MAP.md` (or matching open/closed titles); appends instead of truncating the map
- **Dependencies** — appends a Tracking footer with resolved `#N` links for `epic` / `blocked_by`; on re-run, refreshes **only** the `## Tracking` section when it still contains `(not filed yet)` (use `FORCE_REFRESH=1` to rewrite Tracking anyway)
- **Links** — rewrites relative markdown links to `https://github.com/<repo>/blob/<ref>/...` using the repo **default branch** (override with `FILE_ISSUES_REF=<sha-or-branch>`). Prefer running after merge to the default branch so links stay stable.

## Sizing

Each implementable child targets about **~200 lines** of meaningful change (roughly 150–250). Do not bundle unrelated work.

## Verification (required)

Hardware is **not** the default gate. See [`../test-plan.md`](../test-plan.md) and [`../architecture.md`](../architecture.md):

- **Characterize C behavior → freeze Rust tests → port** (parity first)
- **L0 build** + **L1 symbols** on every C→Rust swap
- **L2 host/unit differential tests** for crypto/pure chunks
- **Domain types** at Rust APIs; raw pointers only in abi/os shims
- **L3 VM insmod** when touching init; **L4 hardware** at wave milestones

Test-infra: `epic-10-test-infra.md`, `test-00-*.md` …  
Architecture: `epic-11-architecture.md`, `arch-00-*.md` …

## Filing cadence

- **Now:** Phase/wave epics + Wave 0–2 children + test-infra epic/children.
- **Later:** open Wave 3–6 / Phase 2 children only when the previous wave is mostly done.

## Status

| ID | File | Status |
|----|------|--------|
| E01–E09 | `epic-*.md` | draft (not on GitHub yet) |
| W0-01 | `wave0-01-docs.md` | done (docs landed; not filed on GitHub) |
| W0-02 | `wave0-02-kbuild.md` | done (KDIR/LLVM + CONFIG_RUST stub link; not filed on GitHub) |
| W0-03 | `wave0-03-scaffold.md` | draft |
| W1-01… | `wave1-*.md` | draft |
| W2-01… | `wave2-*.md` | draft |
| T0 / A0 | `test-00-*.md` / `arch-00-*.md` | done (plan docs + links via W0-01) |
| T1–T3 / A1 | remaining `test-*` / `arch-*` | draft |
