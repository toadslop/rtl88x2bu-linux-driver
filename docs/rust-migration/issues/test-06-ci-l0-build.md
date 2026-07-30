---
title: "[T6] CI: L0 module build against pinned Rust kernel"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T6
epic: E10
blocked_by: [T3]
estimate_loc: 200
---

## Goal

Add a GitHub Actions job that runs the **L0 build gate** on every pull request that can affect the `.ko` output. Today only L2 host tests run in CI; L0 is manual and easy to miss on `hal/`, `os_dep/`, `include/`, or `Makefile` changes that skip the L2 path filter.

## Background

[`test-plan.md`](../test-plan.md) defers L0 in CI until a cached Rust-enabled kernel tree exists. The cloud/dev recipe already pins **v6.12.9** with `CONFIG_RUST=y` and `LLVM=1` ([`dev-environment.md`](../dev-environment.md)). Reuse that pin in CI rather than distro headers.

## Proposed approach

1. Publish (or document how to build) a **CI container image** with:
   - `clang-18` + unsuffixed `llvm-*` / `ld.lld` symlinks
   - `rustc` 1.83 + `rust-src`, `bindgen` 0.65.1
   - Pre-built kernel tree at a fixed path (e.g. `/opt/linux` from v6.12.9)
2. Add `.github/workflows/module-l0.yml` (or extend an existing workflow) that runs:

   ```bash
   export LIBCLANG_PATH=/usr/lib/llvm-18/lib
   make clean
   make KDIR=/opt/linux LLVM=1 -j"$(nproc)"
   ./scripts/ci/verify-ko-probes.sh 88x2bu.ko
   ```

   The probe list lives in [`scripts/ci/verify-ko-probes.sh`](../../../scripts/ci/verify-ko-probes.sh) (also referenced from [`AGENTS.md`](../../../AGENTS.md)); extend it when new Rust objects are linked into `88x2bu.ko`.

3. **Path filters:** trigger on any change under `core/`, `hal/`, `os_dep/`, `include/`, `platform/`, `rust/`, top-level `Makefile`, `dkms.conf`, or the workflow itself. Do **not** require L2 path overlap.

4. **Caching:** prefer baking the kernel into the image (fast, deterministic). If building in-job, cache `$KDIR` with a key tied to the pinned tag (`v6.12.9`).

## Acceptance

- PRs that touch driver build inputs run L0 automatically
- Job fails on compile/link errors or missing Rust objects in `88x2bu.ko`
- [`test-plan.md`](../test-plan.md) updated: L0 marked as CI-automated (not manual-only)
- Document image build/publish steps in [`dev-environment.md`](../dev-environment.md)

## Out of scope

- Matrix builds for multiple distro kernels (see R1)
- Hardware / `insmod` on the runner host

## Notes

- First CI run will be slow if the image is built inline; prioritize a published `ghcr.io` image.
- Arch GCC-built headers are valid for local L0 but are a poor CI default; keep one pinned Clang tree for reproducibility.
