---
title: "[T22] CI: non-default config build matrix + C-only build policy"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T22
epic: E10
blocked_by: []
estimate_loc: 150
---

## Goal

Catch config-only link failures and cfg drift before merge by building a small
matrix of non-default driver configs in the L0 container, and decide what the
`CONFIG_RUST=n` path is.

## Background

The 2026-11 port review (E14) found link failures or silent behaviour changes in:
TDLS, WoWLAN (+GPIO wakeup), `AP_WOWLAN` without `AP_MODE`, `PROC_DEBUG=n` with
`RTW_DEBUG=y`, `POWER_SAVING=n`, `TX_AMSDU` via ccflags, `RTW_80211K`, PCI. None
are built in CI today; only the default 8822BU config is.

The Makefile says C-only builds are unsupported ("no ifndef CONFIG_RUST
fallback"), yet many ports still keep `#if !defined(CONFIG_RUST)` fallbacks and
several recent ports deleted C bodies outright (W4-47/48/49, W3-136, W3-217/218),
so the fallback code is untested dead weight with known link breaks.

## Proposed approach

1. `.github/workflows/module-l0-matrix.yml` (path filter: `Makefile`, `include/**`,
   `rust/**`, `core/**`, `hal/**`): build `88x2bu.ko` with each matrix entry via
   `USER_EXTRA_CFLAGS` / make vars; pass = no modpost undefined symbols.
   Start advisory; make required once green.
2. Decide and document (in [`../../rust-migration.md`](../../rust-migration.md) and
   [`../architecture.md`](../architecture.md)) one of:
   - **(a) Rust required:** delete `!CONFIG_RUST` fallbacks as each TU is fully ported; or
   - **(b) C-only supported:** add a `CONFIG_RUST=n` matrix entry and restore deleted bodies.

## Acceptance

- Matrix job runs on PRs and reports per-config pass/fail.
- The C-only policy is written down; the PR template links it.
- Known failing entries are listed in the job with the FX issue that fixes them.
