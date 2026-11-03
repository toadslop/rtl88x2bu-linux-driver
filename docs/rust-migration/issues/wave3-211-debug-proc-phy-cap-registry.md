---
title: "[W3-211] Translate rtw_debug_rest.c — proc PHY cap registry get/set leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-211
epic: E05
blocked_by: [W3-210]
estimate_loc: 200
---

## Goal

Port registry PHY-capability debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_phy_cap.rs`](../../../rust/rtw_debug_proc_phy_cap.rs) (`CONFIG_80211N_HT`):

- `proc_get_stbc_cap` / `proc_set_stbc_cap`
- `proc_get_rx_stbc` / `proc_set_rx_stbc` (clamp 0–3)
- `proc_get_ldpc_cap` / `proc_set_ldpc_cap`
- `proc_get_txbf_cap` / `proc_set_txbf_cap` (`CONFIG_BEAMFORMING`)

## Notes

- Same lane as [W3-210](wave3-210-debug-phy-cap-dump.md); shares `registry_priv` fields only.
- Does not invoke PHYDM or sounding — registry read/write + printk parity only.

## Acceptance

- L0 + L1 on all listed handlers
- L2: proc_set out-of-range and in-range vectors; proc_get hex/dec lines match C oracle
