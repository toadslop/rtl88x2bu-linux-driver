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

Userspace harness under `tests/host/crypto/` that runs fixed vectors against the C `aes-ctr` implementation, ready to compare with the Rust translation in W1-03.

## Acceptance

- `tests/host` builds on a normal Linux userspace toolchain (no kernel headers)
- aes-ctr vectors pass against current C
- Documented how W1-03 wires the Rust side into the same vectors
- May split vector fixtures vs harness if diff > ~250 LOC
