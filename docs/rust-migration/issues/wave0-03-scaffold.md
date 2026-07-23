---
title: "[W0-03] Add trivial rust/scaffold.rs and call from C init"
labels: [rust-migration, phase-1, wave-0, size/~200]
type: child
id: W0-03
epic: E02
blocked_by: [W0-02]
estimate_loc: 200
---

## Goal

Prove mixed C+Rust link in `88x2bu.ko`: one `extern "C"` symbol from Rust, called once from existing C init (`rtw_drv_entry` or adjacent), with a clear dmesg breadcrumb.

## Deliverables

- `rust/scaffold.rs` (or equivalent path) using RfL/`kernel` as required by the pinned kernel
- One call site in existing C init path
- Makefile lists the new object

## Acceptance

- `insmod` succeeds; dmesg shows scaffold ran
- STA smoke checklist still passes
- Clean `rmmod`

## Out of scope

- Bindgen (Wave 1)
- Translating real driver files
