---
title: "[T19] CI: extend required checks (Rust lint, path-scoped L3)"
labels: [rust-migration, phase-1, size/~100]
type: child
id: T19
epic: E10
blocked_by: [T18]
estimate_loc: 80
---

## Priority

**P2 (hygiene)** — Implement **after** [T18](test-18-branch-protection-enable.md) is enabled, and only when someone has a light CI slice between translation PRs. Do **not** prioritize over open `W3-*` leaf ports.

## Goal

Add **rustfmt** and **path-scoped L3** to the documented required-check set (and branch protection) so high-signal jobs that already run are merge-blocking where appropriate.

## Background

- [`rust-lint.yml`](../../../.github/workflows/rust-lint.yml) runs `check-status-constants.sh` on every PR and `rustfmt --check` when `rust/**` changes ([T16](test-16-ci-rustfmt.md)), but it is **not** listed as a required check in [`contributing.md`](../../contributing.md).
- [`module-l3.yml`](../../../.github/workflows/module-l3.yml) runs on a **narrow PR path set** ([T15](test-15-ci-l3-pr-scope.md)) plus post-merge `master`; when it runs on a PR, failures should block merge.
- `Makefile` / `scripts/ci/verify-ko-probes.sh` changes can break link/init without touching `os_dep/**` — consider modest path expansion (documented, not full L3 on every PR).

## Deliverables

1. Update [`contributing.md`](../../contributing.md) and [`dev-environment.md`](../dev-environment.md#branch-protection) with optional tier:
   - **Always require (with T18):** L0, L1, L2.
   - **Also require:** `Rust lint / rustfmt` and `Module L3 load / module-l3` when GitHub shows them on the PR (path-scoped).
2. Repo admin: add those checks to the branch protection rule once [T18](test-18-branch-protection-enable.md) is live.
3. **Optional** workflow tweak (same PR or follow-up): add PR `paths` for L3 when `Makefile` (Rust object list), `scripts/ci/verify-ko-probes.sh`, or `rust/bindings/**` changes — keep full TCG run; do not require L3 on crypto-only PRs.

## Acceptance

- Docs list the extended checks and when they apply.
- A PR touching `rust/scaffold.rs` cannot merge with red `Module L3 load`.
- A PR touching only `docs/**` still passes all required checks via skip/no-op behavior.

## Out of scope

- L3 on every driver PR (too slow).
- New L2 harnesses or bindgen layout tests ([T20](test-20-ci-clippy-warn.md) is separate).
