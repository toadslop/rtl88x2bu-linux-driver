# Rust migration issue tracker

GitHub Issues are currently **disabled** on this repository, so work items live here until they can be filed.

## How to use

1. Enable Issues: repo **Settings → General → Features → Issues**.
2. From the repo root, run:

```bash
bash docs/rust-migration/issues/file-issues.sh
```

That script creates labels (if needed), opens epic issues, then Wave 0–2 child issues, and writes `ISSUE-MAP.md` with number ↔ file mappings.

## Sizing

Each implementable child targets about **~200 lines** of meaningful change (roughly 150–250). Do not bundle unrelated work.

## Filing cadence

- **Now:** Phase/wave epics + Wave 0–2 children.
- **Later:** open Wave 3–6 / Phase 2 children only when the previous wave is mostly done.

## Status

| ID | File | Status |
|----|------|--------|
| E01–E09 | `epic-*.md` | draft (not on GitHub yet) |
| W0-01… | `wave0-*.md` | draft |
| W1-01… | `wave1-*.md` | draft |
| W2-01… | `wave2-*.md` | draft |
