---
title: "[T2] Host differential test harness for crypto (L2) + aes-ctr vectors"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T2
epic: E10
blocked_by: [T0]
estimate_loc: 220
---

## Goal

Userspace characterization harness under `tests/host/crypto/` that **freezes current C `aes-ctr` behavior** as vectors, then compares Rust to that oracle in W1-03.

## Acceptance

- `tests/host` builds on a normal Linux userspace toolchain (no kernel headers)
- aes-ctr vectors pass against current C **before** the Rust port lands
- Documented how W1-03 runs the same vectors against typed Rust + ABI shim
- May split vector fixtures vs harness if diff > ~250 LOC
