---
title: "[W1-03] Pilot: exact-translate core/crypto/aes-ctr.c to Rust"
labels: [rust-migration, phase-1, wave-1, size/~200]
type: child
id: W1-03
epic: E03
blocked_by: [W1-02]
estimate_loc: 120
---

## Goal

Exact translation of [`core/crypto/aes-ctr.c`](../../../core/crypto/aes-ctr.c) (~70 LOC C) to Rust with original `extern "C"` symbol names and behavior.

## Deliverables

- `rust/` (or `core/crypto/`) `.rs` equivalent
- Preserve ABI for all exported functions
- Remove or stop compiling the `.c` object once Rust replacement links

## Acceptance

- Module builds/loads
- Encrypted STA path still works (ping after WPA2 associate)
- Diff stays near ~200 meaningful lines

## Out of scope

- Idiomatic crypto redesign
- Translating other crypto files (Wave 2)
