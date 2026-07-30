---
title: "[T14] CI: bindgen drift check (generated.rs freshness)"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T14
epic: E10
blocked_by: [T6]
estimate_loc: 50
---

## Goal

Fail CI when committed [`rust/bindings/generated.rs`](../../../rust/bindings/generated.rs) is stale relative to [`scripts/bindgen_rtw.sh`](../../../scripts/bindgen_rtw.sh) and [`rust/bindings/bindgen_helper.h`](../../../rust/bindings/bindgen_helper.h).

## Background

[`scripts/bindgen_rtw.sh --check`](../../../scripts/bindgen_rtw.sh) regenerates bindings and compares to the committed blob. Nothing in CI invokes it today; manual edits or header drift can merge undetected until L0 fails opaquely or FFI breaks at runtime.

## Proposed approach

1. Add a step to `module-l0.yml` (inside the L0 container, which already has bindgen 0.65.1 and `KDIR=/opt/linux`) **or** a small dedicated workflow triggered on `rust/bindings/**`, `scripts/bindgen_rtw.sh`, and `rust/ffi.rs` changes.
2. Run: `KDIR=/opt/linux ./scripts/bindgen_rtw.sh --check`
3. Document in [`docs/rust-migration.md`](../../rust-migration.md) and [`dev-environment.md`](../dev-environment.md) that CI enforces freshness.

## Acceptance

- Stale `generated.rs` fails CI with the script's existing error message (`run ./scripts/bindgen_rtw.sh and commit`)
- Clean `master` passes the check
- Step is path-scoped or cheap enough not to dominate L0 runtime

## Out of scope

- Expanding the bindgen allowlist surface (separate W1-01 follow-ups)
- Running bindgen on every unrelated PR once T10 lands (use path filters)
