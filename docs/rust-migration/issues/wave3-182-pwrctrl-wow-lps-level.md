---
title: "[W3-182] Translate rtw_pwrctrl.c — WoWLAN LPS level swap"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-182
epic: E05
blocked_by: []
estimate_loc: 80
---

## Goal

Port the WoWLAN LPS level backup/restore helper from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_wow.rs`](../../../rust/rtw_pwrctrl_wow.rs):

- `rtw_wow_lps_level_decide`

## Notes

- Entire function under `CONFIG_WOWLAN` / `CONFIG_LPS`; preserve `#ifdef CONFIG_LPS_1T1R` fields.
- Pure `pwrctrl_priv` field twiddle — no HAL calls.
- Parallel lane root for pwrctrl (disjoint module file).
- Host L2 Rust differential uses `--cfg host_pwrctrl_wow_lps_test` on `rust/rtw_pwrctrl_wow.rs`.

## Acceptance

- L0 + L1 when `CONFIG_WOWLAN` is enabled in the test build
- L2: before/after `wow_en` toggles — `lps_level` / `lps_1t1r` backup fields match C oracle
