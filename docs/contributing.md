# Contributing

This repo is an incremental C→Rust migration of the `88x2bu` kernel module. Every PR should be a small, test-gated slice (~200 LOC of meaningful change).

## Workflow

1. Pick an open issue from [GitHub Issues](https://github.com/toadslop/rtl88x2bu-linux-driver/issues) (label `rust-migration`). Draft spec templates and filing tooling live in [`docs/rust-migration/issues/`](rust-migration/issues/README.md).
2. Read [`docs/rust-migration.md`](rust-migration.md) and [`docs/rust-migration/architecture.md`](rust-migration/architecture.md).
3. **Characterize C behavior → freeze tests → port** (do not port first and add tests later).
4. Use domain types at Rust APIs; confine `unsafe` and raw pointers to ABI/OS shims.
5. Open a pull request against `master`. Lead with a plain-English summary of what changed and why (**max 200 words**); GitHub auto-loads the rest of the checklist from [`.github/PULL_REQUEST_TEMPLATE.md`](../../.github/PULL_REQUEST_TEMPLATE.md). Agents follow [pick-up-work-item § PR descriptions](../.cursor/skills/pick-up-work-item/SKILL.md#pr-descriptions-mandatory).

## Verification gates

| Gate | When | Local docs |
|------|------|------------|
| **L0** | Every PR that can affect the `.ko` | [`test-plan.md`](rust-migration/test-plan.md#l0--build-gate) |
| **L1** | Every C→Rust object swap | [`test-plan.md`](rust-migration/test-plan.md#l1--symbol--abi-gate) |
| **L2** | Pure / leaf units (crypto, IE, chplan, security, wlan_util) | [`test-plan.md`](rust-migration/test-plan.md#l2--host-differential-tests-leaf--crypto-first) |
| **L3** | Init / USB registration changes | [`test-plan.md`](rust-migration/test-plan.md#l3--module-load-without-device) |
| **L4** | Wave milestones only | [`smoke-test.md`](smoke-test.md) |

## Required CI checks (pull requests)

When branch protection is enabled, require these **check-run context strings** on `master` merges via pull request (they match each job’s `name:` in the workflow — use them literally in Settings and in `scripts/admin/*`; GitHub’s Checks UI may group them under a workflow title):

| Check context | Workflow | Runs on PR? | Path-scoped? |
|---------------|----------|-------------|--------------|
| `host-l2` | [`.github/workflows/host-l2.yml`](../../.github/workflows/host-l2.yml) | Yes | Yes — skips heavy steps when out of scope |
| `module-l0` | [`.github/workflows/module-l0.yml`](../../.github/workflows/module-l0.yml) | Yes | Yes — skips heavy steps when out of scope |
| `module-l1` | [`.github/workflows/module-l1.yml`](../../.github/workflows/module-l1.yml) | Yes | Yes — skips heavy steps when out of scope |
| `rustfmt` | [`.github/workflows/rust-lint.yml`](../../.github/workflows/rust-lint.yml) | Yes | Yes — `rustfmt` skipped when `rust/**` unchanged; status-constants step always runs |
| `module-l3` | [`.github/workflows/module-l3.yml`](../../.github/workflows/module-l3.yml) | Yes | Yes — QEMU L3 skipped when init/USB/scaffold paths unchanged |

**Post-merge on `master`:** `module-l3` also runs on `push` when driver/build paths change (backstop after merge).

### Path-scoped PR checks (T10)

On **pull requests**, L0/L1/L2, `rustfmt`, and `module-l3` workflows **always trigger** and report a status check. Job-level [`dorny/paths-filter`](https://github.com/dorny/paths-filter) decides whether to run the full gate or a no-op skip step. Out-of-scope PRs (e.g. docs-only) show **Success** on every required context instead of staying **Waiting for status to be reported**.

**Translation PRs** that touch driver/build paths still run the full gate jobs when relevant files change. Crypto-only or docs-only PRs skip heavy L3/L0 work but still satisfy required checks.

Repo admins: see [Branch protection](rust-migration/dev-environment.md#branch-protection) in [`dev-environment.md`](rust-migration/dev-environment.md) for Settings steps and `scripts/admin/enable-branch-protection.sh` ([T18](rust-migration/issues/test-18-branch-protection-enable.md) / #1795 baseline; [T19](rust-migration/issues/test-19-branch-protection-extended-checks.md) / #1796 adds `rustfmt` + `module-l3`).

## Toolchain and environment

First-time setup (Arch pitfalls, Ubuntu packages, pinned kernel, QEMU L3): [`docs/rust-migration/dev-environment.md`](rust-migration/dev-environment.md).
