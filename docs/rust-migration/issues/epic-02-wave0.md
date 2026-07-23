---
title: "[Epic] Wave 0 — RfL out-of-tree build scaffold"
labels: [rust-migration, phase-1, wave-0]
type: epic
id: E02
blocked_by: []
---

## Goal

Make `88x2bu.ko` build against a Rust-enabled kernel (`CONFIG_RUST=y`) and link a trivial Rust object, with migration/smoke docs. No functional driver rewrite yet.

## Verification gate

Per-PR (W0-01…W0-03): **L0** build with pinned `KDIR` + `LLVM=1`, and **L3** VM `insmod`/`rmmod` without a USB device (see [`test-plan.md`](../test-plan.md)).

Wave milestone (**L4**, optional / when hardware available): iface present → STA smoke → clean `rmmod`. L4 is not required to merge Wave 0 child PRs.

## Children

- W0-01 docs
- W0-02 Makefile/Kbuild `.rs` support
- W0-03 trivial `rust/scaffold.rs` + C call site
