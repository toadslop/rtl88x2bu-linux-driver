# GitHub Releases and DKMS packaging (R1)

This document records how installable artifacts are produced for the C→Rust migration
driver and why we chose **DKMS source tarballs** over pre-built `.ko` files.

## Evaluation

| Approach | Pros | Cons |
|----------|------|------|
| **Source tarball + DKMS** on each meaningful `master` merge | Works on any matching kernel; standard for out-of-tree drivers | User must have `CONFIG_RUST=y` headers and the correct compiler/LLVM contract; noisy while migration is incomplete |
| **Source tarball + DKMS** on a fixed schedule | Same install path; low release volume during Wave 0+ | Snapshot may lag tip of `master` by up to one interval |
| **Pre-built `.ko` per kernel version** | Easy `insmod` on one distro | Matrix explosion; vermagic lock-in; Rust-enabled kernels are still uncommon |
| **Git tag per merge (`vYYYY.MM.DD+<shortsha>`)** | Traceable, immutable | Many releases; semver is unclear during migration |
| **Git tag on wave milestones only** | Fewer, meaningful releases | Does not track ongoing verified work |

**Decision:** publish a **DKMS source release** on a **weekly schedule** (Mondays
06:00 UTC) from the tip of `master`, while the driver is still mid-migration.
Releases are **prereleases**. Skip creating a new release when the tip commit
was already published (no-op weeks with no new merges). Manual dry runs and
on-demand publishes remain available via `workflow_dispatch`.

**L0 gate:** each scheduled or publish run executes the same Module L0 build +
probe verify as [`.github/workflows/module-l0.yml`](../.github/workflows/module-l0.yml)
before packaging. No tarball is produced unless that job succeeds for the checked-out
`master` commit.

**Later:** once the Rust driver is complete and work is mostly refactoring /
performance, switch [`.github/workflows/release.yml`](../.github/workflows/release.yml)
back to per-merge publishing (e.g. `workflow_run` after Module L0 succeeds on
`master` for L0-scoped commits).

We do **not** ship universal pre-built `.ko` binaries until an R2 kernel matrix
exists.

## Version string

Packaging sets `PACKAGE_VERSION` in `dkms.conf` at release time (the tree keeps
`@PKGVER@` as a placeholder):

```text
5.13.1.migration.<UTC-date>.<short-sha>
```

Example: `5.13.1.migration.2026.10.07.a1b2c3d`

[`scripts/install-dkms.sh`](../scripts/install-dkms.sh) uses the same
`5.13.1.migration.*` prefix but **without** the UTC date — only
`5.13.1.migration.<short-sha>` from the current git checkout. Release tarballs add
the date so published DKMS versions sort and remain unique across days; compare
`PACKAGE_VERSION` in `dkms.conf`, not the filename alone.

## Build contract in `dkms.conf`

DKMS invokes:

```text
make -j$(nproc) KVER=${kernelver} KSRC=/lib/modules/${kernelver}/build
```

| Host | `MAKE` line |
|------|-------------|
| **Arch** (GCC-built `CONFIG_RUST` headers) | Set `@MAKE_ENV@` to `env PATH=/usr/bin:/bin LIBCLANG_PATH=/usr/lib` (see `install-dkms.sh`) — **omit** `LLVM=1` |
| **Ubuntu / Clang-built tree** | Set `@MAKE_ENV@` to `env LIBCLANG_PATH=/usr/lib/llvm-18/lib` and add `LLVM=1` to the `make` arguments in `dkms.conf` before install, or build from a pinned tree per [`dev-environment.md`](rust-migration/dev-environment.md) |

The migration **requires** `CONFIG_RUST=y` in the target kernel config. Without it,
the Makefile still builds but Rust objects are omitted — see `install-dkms.sh` checks.

## CI pipeline

1. **Module L0 build** ([`module-l0.yml`](../.github/workflows/module-l0.yml)) continues
   to gate PRs and `master` pushes for L0-scoped paths (independent of releases).
2. **DKMS release** ([`release.yml`](../.github/workflows/release.yml)) runs on a
   **weekly cron** (and optional `workflow_dispatch`). It packages tip of `master`
   and publishes a prerelease unless that commit SHA was already released.
3. [`scripts/ci/package-dkms-release.sh`](../scripts/ci/package-dkms-release.sh)
   substitutes `@PKGVER@`, runs `git archive`, and produces
   `rtl88x2bu-<version>-dkms.tar.gz`.
4. `gh release create` uploads the tarball as a **prerelease** with notes pointing here,
   [`dev-environment.md`](rust-migration/dev-environment.md), and
   [`smoke-test.md`](../smoke-test.md).

Manual dry run (no GitHub Release): **Actions → DKMS release → Run workflow** with
**dry_run** enabled; download the artifact from the workflow run. Set **dry_run**
off for an on-demand publish outside the weekly slot.

## Installing from a release tarball

```bash
tar -xzf rtl88x2bu-*-dkms.tar.gz -C /usr/src/
cd /usr/src/rtl88x2bu-5.13.1.migration.*
# Adjust @MAKE_ENV@ / LLVM=1 per table above, then:
sudo dkms add -m rtl88x2bu -v "$(grep PACKAGE_VERSION dkms.conf | cut -d= -f2 | tr -d '"')"
sudo dkms install -m rtl88x2bu -v … -k "$(uname -r)"
```

For Arch one-shot install from a git checkout, prefer
[`scripts/install-dkms.sh`](../scripts/install-dkms.sh).

## Cadence (resolved)

| Phase | Cadence |
|-------|---------|
| **Now** (incomplete Rust port) | Weekly scheduled prerelease from tip of `master` |
| **After** full Rust driver | Per-merge (L0-qualified) releases; drop or keep schedule as a backup |

Track follow-ups under epic **E12** ([#150](https://github.com/toadslop/rtl88x2bu-linux-driver/issues/150)).
