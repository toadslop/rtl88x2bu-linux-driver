---
title: "[W4-107] Translate hal_dm.c — PHYDM RA registered hook"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-107
epic: E06
blocked_by: []
estimate_loc: 70
---

## Goal

Port `rtw_phydm_ra_registed` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — NULL `psta` logs `RTW_ERR` + `rtw_warn_on(1)`
and returns; `mac_id >= macid_ctl->num` returns without PHYDM call; otherwise
`phydm_ra_registed` and unconditional `dump_sta_info(RTW_DBGDUMP, psta)` (same as C).

## Notes

- **Parallel lane:** disjoint from TRX path ([W4-103](wave4-103-hal-dm-runtime-trx-path.md)) and SetHalODMVar work.
- `dump_sta_info` may remain a C `RTW_PRINT_SEL` export; invoke via existing FFI if already Rust.

## Acceptance

- L0 + L1 symbol parity
- L2: fixture STA with valid `mac_id` records `phydm_ra_registed` RSSI argument

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)