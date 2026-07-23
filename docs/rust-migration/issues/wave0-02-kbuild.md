---
title: "[W0-02] Kbuild: allow linking a Rust object into 88x2bu"
labels: [rust-migration, phase-1, wave-0, size/~200]
type: child
id: W0-02
epic: E02
blocked_by: [W0-01]
estimate_loc: 200
---

## Goal

Extend the out-of-tree build so `88x2bu` can include `.rs` objects when built with `make KDIR=<rust-enabled-kernel> LLVM=1` (RfL out-of-tree pattern).

## Deliverables

- Makefile/Kbuild changes only (or minimal stub object if required to keep the build green)
- Document exact `make` invocation in `docs/rust-migration.md`

## Acceptance

- **L0:** builds with pinned `KDIR` + `LLVM=1`
- **L3:** VM `insmod`/`rmmod` without a USB device (no WARN/Oops); do not rely on a hardware plug-in for this gate
- C-only behavior unchanged if no Rust logic added yet
- L4 hardware STA smoke is a Wave 0 milestone, not required for this PR if L0+L3 pass

## Out of scope

- Real translation of driver logic
- `module!` entry (Wave 6)
