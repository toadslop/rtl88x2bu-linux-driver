---
title: "[T8] CI: L3 insmod/rmmod in QEMU on master merges"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T8
epic: E10
blocked_by: [T6]
estimate_loc: 250
---

## Goal

Run the **L3 module load gate** in CI after merges to `master` (and optionally on PRs that touch init/USB registration). This catches link/init/exit regressions that L2 host tests cannot see.

## Background

[`dev-environment.md`](../dev-environment.md#reliable-fallback-busybox-initramfs--qemu-tcg) documents a busybox initramfs + QEMU (TCG) recipe booting the same pinned `bzImage` as `KDIR`. A clean pass shows `registered new interface driver rtl88x2bu` on `insmod` and clean `rmmod` with no Oops/WARN.

## Proposed approach

1. Workflow triggers on `push` to `master` (not every PR — TCG is slow; ~several minutes).
2. Reuse the L0 CI image (kernel tree + `bzImage` + module built in-job or from L0 artifact).
3. Steps (high level):
   - Build or fetch `88x2bu.ko` (L0 artifact)
   - Assemble minimal initramfs with `busybox`, `insmod`, `rmmod`, `dmesg`
   - `qemu-system-x86_64 -kernel $bzImage -initrd … -append "console=ttyS0" -nographic`
   - Script: `insmod /88x2bu.ko` → grep dmesg for registration → `rmmod 88x2bu` → fail on WARN/Oops
4. Optional: also run on PRs when `os_dep/`, `core/rtw_drv.c`, `include/drv_conf.h`, or `rust/scaffold.rs` change (label `l3-required`).

## Acceptance

- Every `master` push runs L3 and posts a visible pass/fail
- Failure artifacts include `dmesg` capture
- Document runtime expectations and QEMU flags in [`dev-environment.md`](../dev-environment.md)
- [`test-plan.md`](../test-plan.md) updated: L3 marked as CI on merge (manual still OK for init-touching PRs pre-merge)

## Out of scope

- USB device emulation / TX-RX
- KVM (not available on `ubuntu-latest`)

## Risks

- Flaky TCG timeouts → set generous job timeout; retry once on infra failure only.
