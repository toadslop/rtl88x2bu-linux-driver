# GitHub Releases and DKMS packaging (R1)

This document records how installable artifacts are produced for the C→Rust migration
driver and why we chose **DKMS source tarballs** over pre-built `.ko` files.

## Evaluation

| Approach | Pros | Cons |
|----------|------|------|
| **Source tarball + DKMS** on each meaningful `master` merge | Works on any matching kernel; standard for out-of-tree drivers | User must have `CONFIG_RUST=y` headers and the correct compiler/LLVM contract |
| **Pre-built `.ko` per kernel version** | Easy `insmod` on one distro | Matrix explosion; vermagic lock-in; Rust-enabled kernels are still uncommon |
| **Git tag per merge (`vYYYY.MM.DD+<shortsha>`)** | Traceable, immutable | Many releases; semver is unclear during migration |
| **Git tag on wave milestones only** | Fewer, meaningful releases | Does not track every verified merge |

**Decision:** publish a **DKMS source release** after **Module L0** succeeds on
`master` for commits that touch L0-scoped paths (same path set as
[`.github/workflows/module-l0.yml`](../.github/workflows/module-l0.yml)). Releases
are **prereleases** until maintainers confirm cadence (see open question on
[#155](https://github.com/toadslop/rtl88x2bu-linux-driver/issues/155)).

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

1. **Module L0 build** ([`module-l0.yml`](../.github/workflows/module-l0.yml)) runs on
   `master` pushes (and PRs) for L0-scoped paths.
2. **DKMS release** ([`release.yml`](../.github/workflows/release.yml)) runs on
   `workflow_run` when L0 completes successfully on `master`, but only if the commit
   changed L0-scoped files (docs-only merges skip both a real L0 build and release).
3. [`scripts/ci/package-dkms-release.sh`](../scripts/ci/package-dkms-release.sh)
   substitutes `@PKGVER@`, runs `git archive`, and produces
   `rtl88x2bu-<version>-dkms.tar.gz`.
4. `gh release create` uploads the tarball as a **prerelease** with notes pointing here,
   [`dev-environment.md`](rust-migration/dev-environment.md), and
   [`smoke-test.md`](../smoke-test.md).

Manual dry run (no GitHub Release): **Actions → DKMS release → Run workflow** with
**dry_run** enabled; download the artifact from the workflow run.

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

## Maintainer open question

> Release on **each L0-qualified merge** (current automation) vs **tag-only**
> releases — confirm before turning off `prerelease`.

Track follow-ups under epic **E12** ([#150](https://github.com/toadslop/rtl88x2bu-linux-driver/issues/150)).
