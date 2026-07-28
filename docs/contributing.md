# Contributing

This repo is an incremental C→Rust migration of the `88x2bu` kernel module. Every PR should be a small, test-gated slice (~200 LOC of meaningful change).

## Workflow

1. Pick an open issue from [GitHub Issues](https://github.com/toadslop/rtl88x2bu-linux-driver/issues) (label `rust-migration`). Draft spec templates and filing tooling live in [`docs/rust-migration/issues/`](rust-migration/issues/README.md).
2. Read [`docs/rust-migration.md`](rust-migration.md) and [`docs/rust-migration/architecture.md`](rust-migration/architecture.md).
3. **Characterize C behavior → freeze tests → port** (do not port first and add tests later).
4. Use domain types at Rust APIs; confine `unsafe` and raw pointers to ABI/OS shims.
5. Open a pull request against `master`. GitHub auto-loads the checklist from [`.github/PULL_REQUEST_TEMPLATE.md`](../../.github/PULL_REQUEST_TEMPLATE.md).

## Verification gates

| Gate | When | Local docs |
|------|------|------------|
| **L0** | Every PR that can affect the `.ko` | [`test-plan.md`](rust-migration/test-plan.md#l0--build-gate) |
| **L1** | Every C→Rust object swap | [`test-plan.md`](rust-migration/test-plan.md#l1--symbol--abi-gate) |
| **L2** | Pure / leaf units (crypto, IE, chplan, security, wlan_util) | [`test-plan.md`](rust-migration/test-plan.md#l2--host-differential-tests-leaf--crypto-first) |
| **L3** | Init / USB registration changes | [`test-plan.md`](rust-migration/test-plan.md#l3--module-load-without-device) |
| **L4** | Wave milestones only | [`smoke-test.md`](smoke-test.md) |

## Required CI checks (pull requests)

GitHub shows status checks as **Workflow name / job id**. When branch protection is enabled, require these three on `master` merges via pull request:

| Check name | Workflow | Runs on PR? | Path-filtered? |
|------------|----------|-------------|----------------|
| `Host L2 tests / host-l2` | [`.github/workflows/host-l2.yml`](../../.github/workflows/host-l2.yml) | Yes | Yes |
| `Module L0 build / module-l0` | [`.github/workflows/module-l0.yml`](../../.github/workflows/module-l0.yml) | Yes | Yes |
| `Module L1 symbols / module-l1` | [`.github/workflows/module-l1.yml`](../../.github/workflows/module-l1.yml) | Yes | Yes |

**Post-merge on `master` (not a PR gate):** `Module L3 load / module-l3` runs after merge when driver/build paths change. See [`.github/workflows/module-l3.yml`](../../.github/workflows/module-l3.yml).

### Path filters and required checks

These workflows use **workflow-level** `paths:` filters. When a PR does not touch matching paths (e.g. docs-only), the workflow **does not run** — the check stays **Waiting for status to be reported** and branch protection **blocks merge** ([GitHub troubleshooting docs](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/collaborating-on-repositories-with-code-quality-features/troubleshooting-required-status-checks)).

Until workflows are refactored (e.g. always trigger with job-level `if:` + `dorny/paths-filter` so out-of-scope jobs report Success), admins enabling these required checks should plan for:

- **Docs-only PRs** — use an admin bypass, ruleset path exception, or temporarily relax required checks.
- **Translation PRs** — all three checks should run when driver/build paths change.

Repo admins: see [Branch protection](rust-migration/dev-environment.md#branch-protection) in [`dev-environment.md`](rust-migration/dev-environment.md) for Settings steps.

## Toolchain and environment

First-time setup (Arch pitfalls, Ubuntu packages, pinned kernel, QEMU L3): [`docs/rust-migration/dev-environment.md`](rust-migration/dev-environment.md).
