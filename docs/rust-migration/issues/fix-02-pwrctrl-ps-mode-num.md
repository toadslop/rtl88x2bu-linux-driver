---
title: "[FX-02] pwrctrl_pm: PS_MODE_NUM is 9 in Rust, 11 in C (modes 9/10 rejected)"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-02
epic: E14
blocked_by: []
estimate_loc: 60
---

## Priority

**P1 (high)** — reachable in the default build via `iwpriv`, proc, and module param.

## Problem

[`rust/rtw_pwrctrl_pm.rs`](../../../rust/rtw_pwrctrl_pm.rs) line 37:
`const PS_MODE_NUM: u8 = 9;` used by `rtw_pm_set_lps` (line ~290, 69104a0c #1605)
and `rtw_pm_set_wow_lps` (line ~341, fbc2d495 #1646) as `if mode >= PS_MODE_NUM { EINVAL }`.

C ([`include/rtw_pwrctrl.h`](../../../include/rtw_pwrctrl.h) `enum Power_Mgnt`):
`… PS_MODE_WWLAN = 8, PM_Radio_Off = 9, PM_Card_Disable = 10, PS_MODE_NUM = 11`.
The C fallback checks `if (mode < PS_MODE_NUM)`.

Impact: `iwpriv wlan0 pm_set lps=9|10` and the `ps_info` proc write now return
-EINVAL; an `rtw_power_mgnt` module param of 9/10 is silently ignored; same for
`wow_lps=` with `CONFIG_WOWLAN`.

L2 missed it because the host oracle header
[`tests/host/include/host_pwrctrl_pm.h`](../../../tests/host/include/host_pwrctrl_pm.h)
copies the same wrong `#define PS_MODE_NUM 9`.

## Fix

1. `const PS_MODE_NUM: u8 = 11;` with a doc comment naming `enum Power_Mgnt`
   (or import the value once T23's bindgen drift check exports enums).
2. Fix the host header to 11 and add L2 vectors for modes 8, 9, 10, 11.

## Acceptance

- L0; L1 unchanged for `rust/rtw_pwrctrl_pm.o`.
- L2: `pm_set lps=10` accepted, `lps=11` rejected, for both Rust and C oracle.

## Notes

- Rust/test-only change; no `core/rtw_pwrctrl.c` edits, so this is parallel to FX-01.
