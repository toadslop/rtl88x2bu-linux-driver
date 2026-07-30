---
title: "[T16] CI: rustfmt --check on rust/** changes"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T16
epic: E10
blocked_by: [T3]
estimate_loc: 60
---

## Goal

Add a lightweight **Rust formatting gate** so `rust/**` changes cannot merge with unformatted code.

## Background

The L0 Docker image installs `rustfmt` (see [`.github/docker/l0/Dockerfile`](../../../.github/docker/l0/Dockerfile)), but no workflow runs `cargo fmt --check` or `rustfmt --check`. As the Rust surface grows, inconsistent formatting adds review noise.

## Proposed approach

1. Add a job to `host-l2.yml` or a small `rust-lint.yml` workflow on `rust/**` changes:
   ```bash
   rustup toolchain install 1.83.0 --profile minimal --component rustfmt
   rustfmt --edition 2021 --check rust/**/*.rs
   ```
   (Adjust glob/edition to match kbuild `RUSTFLAGS` / project convention.)
2. Optionally add `clippy` in a follow-up — start with fmt only to keep signal high and noise low.
3. If the tree is not yet fully formatted, one-time `rustfmt` commit can land before enabling the check.

## Acceptance

- PR that misformats a `rust/*.rs` file fails CI with a clear rustfmt diff hint
- `master` passes after any one-time formatting baseline commit
- Document command in [`dev-environment.md`](../dev-environment.md) for local pre-push

## Out of scope

- `clippy` (separate issue if needed)
- Formatting C code
- `miri` / sanitizers for kernel `unsafe`
