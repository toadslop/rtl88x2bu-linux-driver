---
title: "[Epic] Wave 1 — Bindgen / FFI seam + pilot"
labels: [rust-migration, phase-1, wave-1]
type: epic
id: E03
blocked_by: [E02]
---

## Goal

Establish header→Rust bindings and prove one small `.c` can be replaced by an `extern "C"` Rust translation unit in the same module.

## Smoke gate

Wave 0 checklist, plus pilot symbols exercised (encrypted path still works if crypto pilot lands).

## Children

- W1-01 bindgen script + allowlisted bindings
- W1-02 `rust/ffi` ownership docs/module
- W1-03 pilot translate `core/crypto/aes-ctr.c`
- W1-04 pilot Makefile swap + smoke (may merge with W1-03 if still ~200 LOC)
