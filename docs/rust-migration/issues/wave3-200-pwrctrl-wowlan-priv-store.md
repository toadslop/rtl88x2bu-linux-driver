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

Move the `CONFIG_WOWLAN` cases of `rtw_rust_init_store_u8` from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) into
[`rust/rtw_pwrctrl_init.rs`](../../../rust/rtw_pwrctrl_init.rs) (extend existing init module):

- Fields **case 18–26** (`wowlan_mode`, `wowlan_ap_mode`, `wowlan_p2p_mode`, `wowlan_in_resume`, `wowlan_last_wake_reason`, `wowlan_power_mgmt`, `wowlan_lps_level`, `default_patterns_en`, … through the end of the wowlan block in the switch)

## Notes

- Debug/proc store path — complements ioctl setters in in-flight [W3-196](wave3-196-pwrctrl-wow-pm-setters.md) (`rtw_pm_set_wow_*`), not a duplicate.
- Keep non-wowlan cases in C shim until a later init slice.
- **pwrctrl** lane; chains on W3-196 so PM + priv-store land in order.

## Acceptance

- L0 under `CONFIG_WOWLAN`
- L1: store helper symbol(s) unchanged at link boundary
- L2: host stub — each wowlan field index writes expected `pwrctrl_priv` byte
