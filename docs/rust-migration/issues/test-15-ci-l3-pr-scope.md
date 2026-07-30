---
title: "[T15] CI: path-scoped L3 on PRs for init/USB/scaffold changes"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T15
epic: E10
blocked_by: [T8]
estimate_loc: 150
---

## Goal

Run the **L3 QEMU insmod/rmmod gate** on pull requests that touch high-risk init/USB/scaffold paths, so link/init regressions are caught **before** merge — not only on post-merge `master` runs ([T8](test-08-ci-l3-qemu.md)).

## Background

[`module-l3.yml`](../../../.github/workflows/module-l3.yml) triggers on `push` to `master` only (~3–10 min TCG). The test plan says L3 is required when init/USB registration changes, but PR authors get no automated L3 feedback today. T8 notes this as optional follow-up.

## Proposed approach

1. Add `pull_request` trigger with path filter, e.g.:
   - `os_dep/**`
   - `rust/scaffold.rs`, `rust/kbuild_stub.rs`, `rust/ffi.rs`
   - `include/drv_conf.h`, `include/autoconf.h` (if present)
   - `core/rtw_drv.c` or module entry / USB table sources as identified in codebase
2. Reuse existing `run-l3-qemu.sh` + L0 container image (same as T8).
3. Keep full `master` post-merge L3 as the backstop for all driver-path merges.
4. Optional: `concurrency: cancel-in-progress` on PR workflows to avoid stacked TCG runs.

## Acceptance

- PR changing `rust/scaffold.rs` or `os_dep/linux/usb_intf.c` (or equivalent entry path) runs L3 and must pass before merge (once branch protection includes it)
- Translation-only PRs (crypto/chplan/security) do not run L3 on PR by default
- Failure uploads `l3-serial.log` artifact (same as T8)
- [`test-plan.md`](../test-plan.md) and PR template mention when PR-scoped L3 applies

## Out of scope

- USB device emulation in QEMU
- Requiring L3 on every PR (too slow)
- Weekly scheduled L3 (optional future hygiene)
