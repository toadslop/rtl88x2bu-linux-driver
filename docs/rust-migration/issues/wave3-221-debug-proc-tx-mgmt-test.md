---
title: "[W3-221] Translate rtw_debug_rest.c — proc TX mgmt inject test leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-221
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port IEEE 802.11w test TX procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_tx_mgmt.rs`](../../../rust/rtw_debug_proc_tx_mgmt.rs) (`CONFIG_IEEE80211W`):

- `proc_get_tx_sa_query` / `proc_set_tx_sa_query`
- `proc_get_tx_deauth` / `proc_set_tx_deauth`
- `proc_get_tx_auth` / `proc_set_tx_auth`

## Notes

- Entire block is `#ifdef CONFIG_IEEE80211W`; deauth/auth AP fan-out paths additionally require
  `CONFIG_AP_MODE` at compile time.
- Setters read a single hex digit (`sscanf(..., "%x", ...)`) from a ≤2-byte write — not MAC strings.
  They branch on STA vs AP fwstate and call `issue_action_SA_Query` / `issue_deauth_11w` /
  `issue_auth_11w` (thin FFI from Rust).
- Getters print `__func__` only; include in L1 for symbol parity, L2 optional smoke.
- L2: invalid `count`/sscanf paths; key_type clamp on deauth; record issue_* calls for fixture mlme state.
- Disjoint from [W3-220](wave3-220-debug-proc-monitor-xmit.md) Rust module.

## Acceptance

- L0 with `CONFIG_IEEE80211W` (and AP symbols when testing AP fan-out branches)
- L1 on all six handlers
- L2: hex key_type vectors; STA-mode sa_query/deauth with stub mlme + securitypriv BIP installed
