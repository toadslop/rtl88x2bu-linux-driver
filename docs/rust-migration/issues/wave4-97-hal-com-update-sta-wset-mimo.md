---
title: "[W4-97] Translate hal_com_rest.c — STA wireless set and MIMO type"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-97
epic: E06
blocked_by: []
estimate_loc: 150
---

## Goal

Port STA capability helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs) (or `hal_com_sta.rs` if split):

- `rtw_hal_update_sta_wset`
- `rtw_hal_update_sta_mimo_type` — NSS → `RF_*T*R` mapping + optional `CONFIG_CTRL_TXSS_BY_TP` hook via FFI

## Notes

- **Parallel lane:** `hal_com` / `rust/hal_com.rs` — no overlap with hal_dm tranche 18 PRs.
- Does not port `rtw_hal_update_sta_smps_cap` / rate mask (follow-up slice).

## Acceptance

- L0 + L1 for both symbols
- L2: host vectors for 1T1R, 2T2R, and 11n/ac wireless_mode bitmask → `support_wireless_set` / `mimo_type`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)