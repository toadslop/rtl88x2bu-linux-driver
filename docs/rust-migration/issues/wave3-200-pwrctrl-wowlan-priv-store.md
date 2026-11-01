---
title: "[W3-200] Translate rtw_pwrctrl.c — WoWLAN priv field store (debug path)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-200
epic: E05
blocked_by: [W3-196]
estimate_loc: 150
---

## Goal

Move the wowlan-related `rtw_rust_init_store_u8` switch cases from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) into
[`rust/rtw_pwrctrl_init.rs`](../../../rust/rtw_pwrctrl_init.rs) (extend existing init module):

- **Cases 18–22** (always compiled): `wowlan_mode`, `wowlan_ap_mode`, `wowlan_p2p_mode`, `wowlan_in_resume`, `wowlan_last_wake_reason`
- **Cases 23–26** (inside `#ifdef CONFIG_WOWLAN`): `wowlan_power_mgmt`, `wowlan_lps_level`, `default_patterns_en`, `wowlan_aoac_rpt_loc`

## Notes

- Debug/proc store path — complements ioctl setters in in-flight [W3-196](wave3-196-pwrctrl-wow-pm-setters.md) (`rtw_pm_set_wow_*`), not a duplicate.
- Cases 18–22 are **not** wrapped in `CONFIG_WOWLAN` in C; only 23–26 are. Port and test both Kconfig shapes accordingly.
- Keep non-wowlan cases in C shim until a later init slice.
- **pwrctrl** lane; chains on W3-196 so PM + priv-store land in order.

## Acceptance

- L0: default build covers cases 18–22; repeat with `CONFIG_WOWLAN=y` so cases 23–26 compile
- L1: store helper symbol(s) unchanged at link boundary
- L2: host stub — indices 18–22 on default matrix; 23–26 only when `CONFIG_WOWLAN` is enabled
