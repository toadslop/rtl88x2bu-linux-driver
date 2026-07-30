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
- T3 — CI wiring for L2 (done: `.github/workflows/host-l2.yml`)
- T4 — host chplan differential harness (Wave 2 W2-17; implemented on `cursor/w2-17a-chplan-harness-3dd4`)
- T5 — host security + wlan_util differential harness (Wave 3 W3-04+ / W3-08+)
- T6 — CI L0 module build (pinned Rust kernel image)
- T7 — CI L1 symbol/ABI checks
- T8 — CI L3 QEMU insmod/rmmod on `master` merges
- T9 — branch protection, PR template, required checks
- T10 — refactor PR workflow path filters for branch-protection compatibility
- T11 — extend L1 CI for `*-rest` translation units
- T12 — keep `verify-ko-probes.sh` in sync with linked Rust modules
- T13 — close host-l2 path-filter gaps for `*_rest.c` sources
- T14 — bindgen drift check (`generated.rs` freshness)
- T15 — path-scoped L3 on PRs for init/USB/scaffold changes
- T16 — `rustfmt --check` on `rust/**` changes

## Exit criteria

- Every Wave 1+ translation PR can run L0+L1 locally **and in CI**
- Crypto translations run L2 in CI without hardware
- `master` merges run L3 load/unload without hardware
