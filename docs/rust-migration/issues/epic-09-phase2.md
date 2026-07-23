---
title: "[Epic] Phase 2 — Idiomatic Rust / reduce unsafe"
labels: [rust-migration, phase-2]
type: epic
id: E09
blocked_by: [E01]
---

## Goal

After Phase 1 is green on hardware: adopt RfL safe abstractions (`kernel::usb`, sync types, eventual netdev/cfg80211 helpers), tighten ownership around `dvobj` / `_adapter`, and shrink `unsafe` / `extern "C"` between Rust-only units.

## Children

**Not filed yet.** Slice into ~200 LOC issues only after Phase 1 exit criteria are met.
