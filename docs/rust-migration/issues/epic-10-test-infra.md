---
title: "[Epic] Offline test infrastructure for Rust migration"
labels: [rust-migration, phase-1]
type: epic
id: E10
blocked_by: []
---

## Goal

Provide hardware-free verification (L0–L3) so each ~200 LOC translation PR can merge with reasonable confidence. See [`docs/rust-migration/test-plan.md`](../test-plan.md).

## Children

- T0 — land/keep test-plan doc + PR checklist
- T1 — symbol/ABI check script
- T2 — host crypto differential harness + aes-ctr vectors
- T3 — CI wiring for L2 (and L0 when a Rust kernel image exists)
- T4 — host chplan differential harness (Wave 2 W2-17; implemented on `cursor/w2-17a-chplan-harness-3dd4`)
- T5 — host security + wlan_util differential harness (Wave 3 W3-04+ / W3-08+)

## Exit criteria

- Every Wave 1+ translation PR can run L0+L1 locally
- Crypto translations run L2 in CI without hardware
