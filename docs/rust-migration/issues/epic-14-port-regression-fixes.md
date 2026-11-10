---
title: "[Epic] Rust port regression fixes (2026-11 port review)"
labels: [rust-migration, phase-1, regression]
type: epic
id: E14
blocked_by: []
---

## Goal

Fix the behaviour, build, and ABI regressions found by the deep review of the
C→Rust ports merged to `master` between 2026-10-10 and 2026-11-10 (tip
`aa3a7b43`), and add the build/CI guards that would have caught them.

Porting rules distilled from the review: [`../porting-guide.md`](../porting-guide.md).

## Context

The review compared every ported function against `git show <port>^:<file>.c`,
diffed hand-written `extern "C"` declarations against bindgen output of the real
C, compared the rustc `--cfg` set against the effective C macro set, checked
hard-coded struct offsets with `offsetof` under the real module flags, and
inventoried `core::panicking` call sites in `extern "C"` functions.

Recurring root causes:

1. **Hand-copied C facts** — constants, struct offsets, and `systime` width
   retyped in Rust instead of generated or exported from C.
2. **Config parity by grep** — `rustflags-y += --cfg …` derived from Makefile
   variables or `autoconf.h` greps, not from the effective C macro set.
3. **Wrapping vs overflow-checked arithmetic** — C relied on modular arithmetic;
   `CONFIG_RUST_OVERFLOW_CHECKS=y` turns the same Rust `+`/`-`/`<<` into a panic.
4. **FFI null policy not matching the C contract** — null rejected where C allows
   it, or checked on the wrong pointer.
5. **L2 tests that do not exercise the shipped code** — host-only rewrites and
   copied constants instead of the kernel object and the `commit^` C oracle.

## Children

High (default build or common kernel configs):

- FX-01 — pwrctrl rpwm/cpwm/init time shims truncate `systime` to u32
- FX-02 — `PS_MODE_NUM` is 9 in Rust, 11 in C
- FX-03 — `dump_adapters_status(RTW_DBGDUMP, …)` prints nothing

Medium (supported non-default configs, or default-build behaviour change):

- FX-04 — `rtw_set_ps_mode` drops TDLS PS null-data notifications
- FX-05 — hard-coded `_ADAPTER` byte offsets → C-exported offsets
- FX-06 — hal_com: stop force-enabling `CONFIG_RTW_CUSTOMER_STR`; gate Rust on C config
- FX-07 — Makefile rustc cfg / object-link parity fixes
- FX-08 — debug proc Rust cfg gates vs C helper guards (link safety)
- FX-09 — hal shim guards: `CONFIG_LPS_ACK`, `rtw_malloc` macro, recv free NULL
- FX-10 — hal_dm narrowband `else` now skips all phydm hooks

Low (latent, debug-only, log-only):

- FX-11 — overflow-checked arithmetic where C wrapped (pwrctrl wakeup/lps_tp, recv bh dbg)
- FX-12 — `rtw_sec_read_cam_ent` NULL `mac`/`key` + offset write
- FX-13 — debug proc setter/print parity (`ips_mode`, `%d` parsing, warn/errno)
- FX-14 — pwrctrl: restore `dbg_ps_insuspend_cnt` and dropped PS diagnostics
- FX-15 — hal_halmac / hal_dm: shift panic, mapper warnings, `RTW: RTW:` prefix
- FX-16 — beamforming fast path sets `sounding_dim` in beamformer branch
- FX-17 — hand-written `extern "C"` signature drift (narrow args, `systime`, duplicates)
- FX-18 — debug proc input validation (pre-existing C bugs carried into helpers)
- FX-19 — robustness: `c2h_evt_read_88xx` length clamp, `WapiSMS4Cryption(len=0)`

Build / CI / test guards (filed under [E10](epic-10-test-infra.md)):

- T21 — generate rustc `--cfg` from the effective C macro set + parity check
- T22 — CI: non-default config build matrix + C-only build policy
- T23 — CI: `extern "C"` signature drift check + panic-site inventory
- T24 — L2 fidelity: test shipped Rust cores, full-width clocks, real C oracles

## Exit criteria

- All FX children closed; default L0/L1/L2/L3 gates green.
- T21–T23 checks run in CI so the same classes fail a PR instead of shipping.
