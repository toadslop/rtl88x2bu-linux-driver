---
title: "[W3-177] Translate rtw_recv.c — AP pwrbit and WMM PS data handlers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-177
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port AP-mode power-save frame hooks from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to
[`rust/rtw_recv_ap_ps.rs`](../../../rust/rtw_recv_ap_ps.rs):

- `process_pwrbit_data`
- `process_wmmps_data`

## Notes

- Wrapped in `CONFIG_AP_MODE` (and `CONFIG_TDLS` branches inside WMM PS); preserve guards.
- Calls `stop_sta_xmit` / `wakeup_sta_to_xmit` via existing C FFI — do not pull xmit queue logic into Rust.
- Independent recv lane (disjoint from `rtw_recv_bh_dbg.rs` and LLC paths in [W3-46](wave3-46-recv-llc-ethhdr.md)).

## Acceptance

- L0 + L1 with `CONFIG_AP_MODE` enabled in the CI/default autoconf profile
- L2: host harness with stub `sta_info` + synthetic QoS data frames (pwr bit set/clear, U-APSD trigger paths)
