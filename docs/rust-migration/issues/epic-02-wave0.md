---
title: "[Epic] Wave 0 — RfL out-of-tree build scaffold"
labels: [rust-migration, phase-1, wave-0]
type: epic
id: E02
blocked_by: []
---

## Goal

Make `88x2bu.ko` build against a Rust-enabled kernel (`CONFIG_RUST=y`) and link a trivial Rust object, with migration/smoke docs. No functional driver rewrite yet.

## Smoke gate

`make KDIR=… LLVM=1` → `insmod` → iface present → existing STA smoke still passes → clean `rmmod`.

## Children

- W0-01 docs
- W0-02 Makefile/Kbuild `.rs` support
- W0-03 trivial `rust/scaffold.rs` + C call site
