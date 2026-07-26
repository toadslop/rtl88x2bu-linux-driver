---
title: "[T5] Host security + wlan_util differential harness"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T5
epic: E10
blocked_by: [T2]
estimate_loc: 200
---

## Goal

Extend offline test infrastructure for Wave 3 security and wlan_util ports (mirrors T2/T4 pattern):

- `tests/host/security/` — C oracle + vectors for `security_type_str`, WEP primitives, TKIP MIC
- `tests/host/ie/` — IE parse vectors (may land with W3-03 before T5 is fully wired)
- `tests/host/wlan_util/` — rate-byte classification and ratetbl conversion vectors
- `make -C tests/host/security test` (and sibling targets)

## Acceptance

- Vectors run against C oracle and Rust staticlib/objects
- Wired into PR checklist for W3-04+ security and W3-08+ wlan_util issues
- Document in [`test-plan.md`](../test-plan.md) L2 section
