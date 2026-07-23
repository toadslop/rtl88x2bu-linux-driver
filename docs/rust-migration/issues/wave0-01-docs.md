---
title: "[W0-01] Document Rust migration + smoke-test checklist"
labels: [rust-migration, phase-1, wave-0, size/~200]
type: child
id: W0-01
epic: E02
estimate_loc: 150
---

## Goal

Add migration and verification docs so every later PR uses the same gate.

## Deliverables

- `docs/rust-migration.md` — two phases, ABI rules, pinned Rust-enabled `KDIR`, `LLVM=1`, rtw88 blacklist note
- `docs/smoke-test.md` — commands: build, blacklist check, insmod, scan, WPA2 associate, ping, rmmod, replug

## Acceptance

- Docs exist and match the agreed plan (exact translation first; ~200 LOC PRs)
- No driver code behavior change

## Out of scope

- Makefile Rust support (W0-02)
- Scaffold `.rs` (W0-03)
