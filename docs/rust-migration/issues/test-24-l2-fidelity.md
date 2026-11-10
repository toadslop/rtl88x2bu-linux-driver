---
title: "[T24] L2 fidelity: test the shipped Rust cores against real C oracles"
labels: [rust-migration, phase-1, size/~200]
type: child
id: T24
epic: E10
blocked_by: [FX-07]
estimate_loc: 200
---

## Goal

Make L2 host tests exercise the same Rust code the kernel links and compare it
with the **original** C (`git show <port>^:<file>.c`), not with a hand-copied model.

## Background

The 2026-11 port review (E14) found L2 suites that cannot catch the regressions they
were meant to guard:

- `tests/host/include/host_pwrctrl_pm.h` copies `PS_MODE_NUM 9` (FX-02).
- IPS (`rtw_pwrctrl_ips_host.rs`), rpwm (`host_hal::cpwm_polling_loop`), cpwm_int
  (`rtw_pwrctrl_cpwm_int_host.rs`), ps_processor, ps_rdy, lps_exec test
  `#[cfg(host_*)]` rewrites instead of the kernel core function.
- `test-lps-exec-rust` is not in the Makefile `test` target.
- Wakeup / pwrctrl fake clocks are `u32`, so `systime` width bugs (FX-01) cannot show.
- WAPI frame oracle (`rust/wapi_sms4_frame_host.rs`, `tests/host/wapi_sms4/wapi_sms4_frame_c_oracle.c`)
  models real encryption while the shipped code early-returns / returns `_FAIL`.

## Proposed approach

1. Route host builds through the shared core fns (`cpwm_polling_core`, the kernel
   `rtw_ips_*` body, …) with the C shims replaced by host stubs; delete the parallel
   host rewrites.
2. Host oracle constants come from the real headers (include
   `include/rtw_pwrctrl.h` enums via a host shim header) — no retyped values.
3. Fake clock is `unsigned long` starting at `0xFFFF_FF00`, with a
   `jiffies_to_msecs` model for HZ=100/250/300/1000.
4. Add `test-lps-exec-rust` to `test`; make the WAPI frame oracle mirror the
   shipped early-return behaviour.

## Acceptance

- Reverting FX-01 or FX-02 makes an L2 target fail.
- `make test` runs every `test-*-rust` target listed in the Makefile.
- [`../test-plan.md`](../test-plan.md) L2 section states "oracle = `commit^` C; Rust under test = kernel core".

## Notes

- **Dependency rationale:** edits Makefile test targets; chained behind FX-07 to avoid Makefile merge conflicts.
