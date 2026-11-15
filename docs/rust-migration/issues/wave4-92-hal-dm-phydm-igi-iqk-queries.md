---
title: "[W4-92] Translate hal_dm.c — PHYDM IGI and IQK status queries"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-92
epic: E06
blocked_by: []
estimate_loc: 160
---

## Goal

Port small PHYDM query helpers from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_get_cur_igi`
- `rtw_phydm_is_iqk_in_progress`

## Notes

- **Parallel lane:** leaf helpers used by debug/diagnostic paths (`SetHalODMVar` RX info dump stays in C for now).
- Delegate to existing `adapter_to_phydm` / `odm` FFI for register reads; no MMIO port in this slice.

## Acceptance

- L0 + L1 symbol parity
- L2 host fixture with mock `dm_struct` returning fixed IGI / IQK-in-progress flags

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)