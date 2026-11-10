---
title: "[FX-01] pwrctrl: rpwm/cpwm/init time shims truncate systime to u32"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-01
epic: E14
blocked_by: []
estimate_loc: 100
---

## Priority

**P1 (high)** — breaks LPS leave on `CONFIG_HZ=300` kernels after 5 minutes of
uptime in the default USB build (`CONFIG_DETECT_CPWM_BY_POLLING` is on).

## Problem

Same bug class as 2feb9b0d (#1594, PS_RDY deny time). The C shims added for the
Rust ports return and take `u32` instead of `systime` (`unsigned long`):

| Shim (`core/rtw_pwrctrl.c`) | Rust caller | Introduced |
|---|---|---|
| `u32 rtw_rust_rpwm_get_current_time(void)` / `u32 rtw_rust_rpwm_get_passing_time_ms(u32)` | [`rust/rtw_pwrctrl_rpwm.rs`](../../../rust/rtw_pwrctrl_rpwm.rs) (`cpwm_polling_core`) | 3851c27a (#1525) |
| `u32 rtw_rust_cpwm_get_current_time(void)` / `u32 rtw_rust_cpwm_get_passing_time_ms(u32)` | [`rust/rtw_pwrctrl_cpwm_int.rs`](../../../rust/rtw_pwrctrl_cpwm_int.rs) (`type Systime = u32`, `LPS_Leave_check`) | a4058ac9 (#1621), df5dfdd1 (#1622) |
| `u32 rtw_rust_init_get_current_time(void)` → `ips_deny_time` / `lps_deny_time` | [`rust/rtw_pwrctrl_init_kernel.rs`](../../../rust/rtw_pwrctrl_init_kernel.rs) | 301f6e00 (#1592) |

On 64-bit, jiffies starts at `(u32)(-300*HZ)` and crosses 2^32 after ~300 s.
After that, `jiffies - (unsigned long)(u32)start` carries an extra 2^32·k.
`jiffies_to_msecs()` cancels it only when 1000/HZ is an integer.

Verified against `kernel/time/time.c` (64-bit path `DIV_ROUND_UP(j * 10, 3)` for
HZ=300): a real 10 ms wait measures **1431655776 ms**, so `rtw_cpwm_polling`
times out after one 1 ms poll ("polling cpwm timeout", `_FAIL`) and
`LPS_Leave_check` gives up immediately. HZ=1000/250/100 happen to cancel.

The `rtw_rust_init_get_current_time` case is benign today (all users go through
`time_after`) but contradicts the #1594 width policy.

## Fix

1. Change the six shims to `systime` in/out; add
   `_Static_assert(sizeof(rtw_rust_…_get_current_time()) == sizeof(systime), …)`
   next to each pair, matching #1594.
2. Rust: declare them with `core::ffi::c_ulong`; delete `type Systime = u32` in
   `rtw_pwrctrl_cpwm_int.rs` (use the kernel-width alias already used by
   `rtw_pwrctrl_wakeup.rs`).
3. Init: add a `systime` store accessor instead of routing deny times through
   `rtw_rust_init_store_u32`.

## Acceptance

- L0 build; L1 symbols unchanged for `rust/rtw_pwrctrl_{rpwm,cpwm_int,init_kernel}.o`.
- L2: host vectors for the polling / leave-check loops use a fake clock that
  starts at `0xFFFF_FF00` and crosses 2^32 during the wait, with a `jiffies_to_msecs`
  model for HZ=300 (fails before the fix, passes after).
- `rg 'fn rtw_rust_(rpwm|cpwm|init)_get_(current|passing)' rust/` shows no `u32`.

## Notes

- Same-TU lane: FX-04 and FX-14 also edit `core/rtw_pwrctrl.c` and are chained behind this issue.
- Related width drift outside pwrctrl (`rtw_recv_sta_validate.rs`, `hal_com_c2h_mac_hidden.rs`) is in FX-17.
