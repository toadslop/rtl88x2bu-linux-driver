---
title: "[T23] CI: extern C signature drift check + panic-site inventory"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T23
epic: E10
blocked_by: []
estimate_loc: 200
---

## Goal

Make two review-time checks from the 2026-11 port review (E14) automatic:

1. **Signature drift** — every hand-written `extern "C"` import in `rust/` matches the
   C definition, and every `#[no_mangle] extern "C"` export matches its C prototype.
2. **Panic inventory** — list `core::panicking::*` call sites reachable from each
   `extern "C"` function so new overflow/bounds panics are visible in review.

## Background

Hand-written bindings drift silently; neither compiler sees both sides (Mainmatter
*C to Rust Migration Book* ch. 1–2). The review's prototype found 20 import
mismatches, 6 inconsistent duplicates (FX-17), `PS_MODE_NUM` 9 vs 11 (FX-02), and u32
`systime` shims (FX-01). With `CONFIG_RUST_OVERFLOW_CHECKS=y`, `+`/`-`/`<<`/indexing
in an `extern "C"` fn becomes a kernel BUG (FX-11, FX-15); objtool already warns
("falls through") for `ccmp_encrypt_pv1_inner`, `dump_sta_info`, `dump_sta_traffic`,
`rtw_aid_alloc`, `rtw_scan_timeout_decision`.

## Proposed approach

1. `scripts/ci/check-ffi-drift.py`: parse `extern "C"` blocks (args/ret) from
   `rust/*.rs` (kernel cfg only); run bindgen with the module's real flags
   (`eval` the `.o.cmd` line) over the defining C files with
   `--allowlist-function` per name and `--no-recursive-allowlist`; normalize
   aliases (`U8`→`u8`, `c_int`→`i32`, `Systime`→`c_ulong`); report mismatches and
   duplicate-but-different declarations. Same for exports vs `include/*.h`.
   Also allowlist enums/consts used by Rust (`Power_Mgnt`, …) so values are compared.
2. `scripts/ci/panic-sites.sh`: `llvm-objdump -dr rust/*.o`, map each
   `panic_*` / `slice_*_fail` relocation to its enclosing exported symbol; diff
   against a committed baseline and fail on new sites in functions touched by the PR.
3. Wire both into `module-l0.yml` (after the build) — advisory first.

## Acceptance

- Drift check output on `master` matches the review counts (then 0 after FX-17).
- Panic baseline committed; adding `a + b` on a C-supplied `u32` in an export shows up as a new site.
- [`../test-plan.md`](../test-plan.md) lists both under L1.
