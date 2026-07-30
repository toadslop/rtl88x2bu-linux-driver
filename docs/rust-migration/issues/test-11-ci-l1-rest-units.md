---
title: "[T11] CI: extend L1 checks for *-rest translation units"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T11
epic: E10
blocked_by: [T7]
estimate_loc: 80
---

## Goal

Run **L1 symbol/ABI checks** in CI for the `*-rest` Rust objects that already have Make targets locally but are omitted from the L1 CI scripts.

## Background

[`scripts/ci/run-l1-unit-checks.sh`](../../../scripts/ci/run-l1-unit-checks.sh) and [`scripts/ci/l1-targets-from-diff.sh`](../../../scripts/ci/l1-targets-from-diff.sh) cover seven aggregate targets. The Makefile also defines:

- `rust-check-symbols-rtw-chplan-rest`
- `rust-check-symbols-rtw-io-rest`
- `rust-check-symbols-rtw-ieee80211-rest`

Changes to `rust/rtw_chplan_rest.rs`, `rust/rtw_io_rest.rs`, or `rust/rtw_ieee80211_rest.rs` do not map to these targets today. A `rust/*` change triggers "full suite" but that suite still skips the three `*-rest` targets.

## Proposed approach

1. Add the three targets to `run-l1-unit-checks.sh` `ALL_TARGETS` / default list.
2. Extend `l1-targets-from-diff.sh` path cases, e.g.:
   - `rust/rtw_chplan_rest.rs`, `core/rtw_chplan_rest.c`, `tests/host/chplan/*rest*` → `rust-check-symbols-rtw-chplan-rest`
   - `rust/rtw_io_rest.rs`, `core/rtw_io_rest.c`, `tests/host/io/**` → `rust-check-symbols-rtw-io-rest`
   - `rust/rtw_ieee80211_rest.rs`, `core/rtw_ieee80211_rest.c`, `tests/host/ie/*rest*` → `rust-check-symbols-rtw-ieee80211-rest`
3. Update the L1 scope table in [`test-plan.md`](../test-plan.md#l1--symbol--abi-gate).

## Acceptance

- `module-l1.yml` runs the relevant `*-rest` target when those sources change
- Full-suite fallback includes all `*-rest` targets
- Local repro: `make KDIR=/opt/linux LLVM=1 rust-check-symbols-rtw-chplan-rest` (and io/ieee80211-rest) pass on `master`

## Out of scope

- Per-crypto L1 targets (crypto relies on L2 host parity + L0 link today)
- New allowlists unless a swap requires one
