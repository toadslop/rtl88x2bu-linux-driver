---
title: "[T12] CI: keep verify-ko-probes in sync with linked Rust modules"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T12
epic: E10
blocked_by: [T6]
estimate_loc: 60
---

## Goal

Extend [`scripts/ci/verify-ko-probes.sh`](../../../scripts/ci/verify-ko-probes.sh) so L0 catches accidental unlinking of newly landed Rust objects.

## Background

L0 runs `verify-ko-probes.sh` after every module build. The probe list is the single source of truth (see [`AGENTS.md`](../../../AGENTS.md)). It is missing symbols that already exist in linked Rust code:

- `rtw_rust_chplan_rest_probe` (`rust/rtw_chplan_rest.rs`)
- `rtw_rust_io_rest_probe` (`rust/rtw_io_rest.rs`)

`rust/rtw_rf_rest.rs` and `rust/rtw_ieee80211_rest.rs` are linked into `88x2bu.ko` but define no `rtw_rust_*_probe` yet — either add probes in the same PR or document intentional omission in the script comment.

## Proposed approach

1. Add missing probe symbols to `PROBES` in `verify-ko-probes.sh`.
2. Add `rtw_rust_rf_rest_probe` and `rtw_rust_ieee80211_rest_probe` to their modules if linked into `88x2bu.ko` (mirror other `rtw_*` units), or document intentional omission in the script comment.
3. Keep [`AGENTS.md`](../../../AGENTS.md), [`test-06-ci-l0-build.md`](test-06-ci-l0-build.md), and any `nm | grep` snippets in docs aligned with the script.

## Acceptance

- `./scripts/ci/verify-ko-probes.sh 88x2bu.ko` passes on a clean L0 build
- Removing a probe symbol from Rust causes L0 CI to fail with a clear missing-symbol message
- Probe list comment documents the contract (one symbol per linked Rust unit or explicit exception)

## Out of scope

- Bit-exact `.ko` comparison
- Exporting non-probe `extern "C"` API symbols (those belong in L1)
