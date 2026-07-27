---
title: "[Epic] Distribution — DKMS packages and GitHub Releases"
labels: [rust-migration, phase-1]
type: epic
id: E12
blocked_by: [T6]
---

## Goal

Ship installable artifacts for users who want pre-packaged driver builds without cloning the repo on every update. Evaluate what "release on each merge" means for a **kernel-specific** out-of-tree module with a **Rust-for-Linux** build requirement.

## Context

- `dkms.conf` exists but uses `PACKAGE_VERSION="@PKGVER@"` and `KSRC=` without `LLVM=1` — needs updating for the migration build contract (`KDIR`, `CONFIG_RUST=y`).
- **0 GitHub Releases** today; version in README is still vendor `5.13.1-30`.
- Pre-built `.ko` files only work for an exact kernel vermagic; most users need **DKMS source packages** that compile on install.

## Children

- R1 — Release strategy + automation (this epic's first child)

## Exit criteria

- Tagged releases (or merge-to-master pipeline) produce at least a **source + DKMS tarball** with a coherent version string
- Install docs cover Arch (GCC kernel) vs Ubuntu/pinned (LLVM) paths
- Release artifacts are clearly labeled with kernel/Rust requirements
