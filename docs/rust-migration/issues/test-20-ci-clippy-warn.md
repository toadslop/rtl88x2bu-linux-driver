---
title: "[T20] CI: clippy on rust/** (warn-only job)"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T20
epic: E10
blocked_by: []
estimate_loc: 120
---

## Priority

**P3 (backlog)** — Nice static-analysis uplift; **defer** while translation throughput matters. [T16](test-16-ci-rustfmt.md) intentionally shipped fmt-only to limit noise. Pick up in a quiet CI slice or when `rust/**` defect rate justifies it.

## Goal

Run **`cargo clippy`** (or `clippy-driver` via pinned `rustc` 1.83) on migration crates in a **non-blocking** CI job first, then tighten to deny selected lints in a later issue if signal stays high.

## Background

- L0 Docker image already installs `clippy` (`.github/docker/l0/Dockerfile`).
- Kernel-linked `rust/*.rs` crates are built by kbuild, not a workspace `Cargo.toml` — the job likely needs a small script that invokes `clippy` per crate or uses `RUSTFLAGS` patterns documented in [`dev-environment.md`](../dev-environment.md).
- L1 catches missing symbols; clippy catches mistaken `unsafe`, useless conversions, and dead code paths earlier.

## Proposed approach

1. Extend [`rust-lint.yml`](../../../.github/workflows/rust-lint.yml) (or add `rust-clippy.yml`) with path filter on `rust/**`.
2. **Phase A (this issue):** `continue-on-error: true` or a dedicated check named e.g. `Rust clippy (advisory)` — **not** required in branch protection.
3. Document local command in `dev-environment.md` matching CI.
4. **Phase B (future issue):** flip to required + deny `clippy::undocumented_unsafe` / `clippy::not_unsafe_ptr_arg_deref` for ffi-heavy crates only.

## Acceptance

- PR changing `rust/**` gets a clippy advisory log artifact or job summary.
- Job does not fail default PRs on existing warnings (baseline allowlist or `-A` for grandfathered lints documented in script).
- [`test-plan.md`](../test-plan.md) mentions clippy as optional static analysis (one paragraph).

## Out of scope

- Miri, sanitizers on kernel `unsafe` (see test-plan “Out of scope”).
- Making clippy a required check (future issue after baseline cleanup).
- Bindgen `--layout-tests` / struct ABI harness (separate future T* if bindgen allowlist grows).
