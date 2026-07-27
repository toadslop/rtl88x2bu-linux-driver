---
title: "[R1] GitHub Releases: versioning, DKMS tarball, merge pipeline"
labels: [rust-migration, phase-1, size/~200]
type: child
id: R1
epic: E12
blocked_by: [T6]
estimate_loc: 200
---

## Goal

Design and implement automated **GitHub Releases** so each meaningful merge (or nightly) produces installable artifacts. Start with an evaluation doc, then minimal automation.

## Evaluation questions (answer in PR / `docs/release.md`)

| Approach | Pros | Cons |
|----------|------|------|
| **Source tarball + DKMS** on every `master` merge | Works on any matching kernel; standard for OOT drivers | User must have `CONFIG_RUST=y` headers + correct compiler |
| **Pre-built `.ko` per kernel version** | Easy `insmod` for one distro | Matrix explosion; vermagic lock-in; Rust kernel rare today |
| **Git tag per merge (`vYYYY.MM.DD+<shortsha>`)** | Traceable, immutable | Many releases; semver unclear during migration |
| **Git tag on wave milestones only** | Fewer, meaningful releases | Does not satisfy "each merge" desire |

**Recommendation to document:** default to **DKMS source release on each `master` push** (or daily cap) with calver + git sha; optional milestone tags for hardware smoke (L4) sign-off. Do **not** promise universal pre-built `.ko` until a kernel matrix exists.

## Implementation sketch

1. **Version string:** e.g. `5.13.1-migration.<date>.<shortsha>` written into `dkms.conf` / module metadata at release time.
2. **Workflow** `.github/workflows/release.yml`:
   - Trigger: `push` to `master` (with `paths-ignore` for docs-only if desired) **or** `workflow_dispatch`
   - Depends on L0 CI job passing (T6)
   - Steps: compute version → `sed` `dkms.conf` → `git archive` → attach `rtl88x2bu-<ver>-dkms.tar.gz` → `gh release create` (draft or prerelease during Phase 1)
3. **Update `dkms.conf`:** use `KVER`/`KSRC` compatible with DKMS, document when users must set `LLVM=1` (Ubuntu) vs omit (Arch GCC).
4. **Release notes template:** kernel pin, `CONFIG_RUST=y` requirement, link to [`dev-environment.md`](../dev-environment.md), smoke-test pointer.

## Acceptance

- At least one successful dry-run release (prerelease) from CI
- `dkms.conf` matches current Makefile contract for Rust builds
- README links to Releases page and states pre-built vs DKMS expectations
- Evaluation section committed (short `docs/release.md` or section in `dev-environment.md`)

## Out of scope (follow-ups)

- **R2** — Multi-kernel binary matrix (Ubuntu 24.04 + Arch when `CONFIG_RUST` common)
- Signing / secure boot
- PPA / COPR packaging

## Open question for maintainers

> "Release on each merge" vs "release on tag only" — confirm cadence before enabling non-prerelease auto-publish.
