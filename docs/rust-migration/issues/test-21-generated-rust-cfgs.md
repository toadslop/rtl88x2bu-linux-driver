---
title: "[T21] Build: generate rustc --cfg from the effective C macro set + parity check"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T21
epic: E10
blocked_by: [FX-07]
estimate_loc: 200
---

## Goal

Stop hand-deriving `rustflags-y += --cfg …` in the Makefile. Generate the Rust cfg
set from the macros C actually sees, and fail CI when a `#[cfg(…)]` used in
`rust/` has no C counterpart (or vice versa) for the configs we build.

## Background

The 2026-11 port review (E14) found 13 classes of cfg/object-link drift (FX-07),
all caused by cfgs derived from Makefile variables or `grep include/autoconf.h`
instead of the effective macro set (`ccflags-y`, `drv_conf.h`, `hal_ic_cfg.h`,
kernel `.config`). Mainmatter *C to Rust Migration Book* ch. 6 (production-quality
FFI: build integration, automated checks) — the FFI surface must be checked
mechanically, not by review.

## Proposed approach

1. `scripts/rust_cfgs.sh`: run `$(CC) $(ccflags-y) -dM -E -include include/drv_conf.h
   -include include/hal_ic_cfg.h -x c /dev/null` (same flags kbuild uses for
   `core/rtw_*.o`), map `CONFIG_FOO` / allowlisted non-`CONFIG_` macros
   (`DBG_*`, `HOST_*` excluded) to `--cfg config_foo`, write
   `$(obj)/rust/generated_cfgs` and pass it via `rustflags-y += @$(obj)/rust/generated_cfgs`.
2. Replace the hand-derived `--cfg` lines; keep explicit lines only for Rust-only
   cfgs (documented allowlist).
3. `scripts/ci/check-rust-cfg-parity.sh`: list every `cfg(config_…)` in `rust/*.rs`
   (kernel path, not `host_*`) and assert the macro exists in at least one
   configuration of the T22 matrix; print cfgs Rust uses that C never defines.

## Acceptance

- Default build: generated cfg set equals the post-FX-07 hand set (diff attached).
- `check-rust-cfg-parity.sh` runs in `rust-lint.yml` (advisory first, then required).
- [`dev-environment.md`](../dev-environment.md) documents how to add a cfg.

## Notes

- **Dependency rationale:** builds on the corrected hand list from FX-07 (same Makefile region) so the diff is reviewable.
