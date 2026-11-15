---
title: "[W4-93] Translate hal_dm.c — min RSSI and PHY counter queries"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-93
epic: E06
blocked_by: []
estimate_loc: 180
---

## Goal

Port PHYDM / DM query helpers from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_dm_get_min_rssi` — walk `macid_ctl` for linked STA min RSSI
- `rtw_phydm_get_min_rssi` — `phydm_cmn_info_query` RSSI min
- `rtw_phydm_get_edcca_flag`
- `rtw_phydm_get_phy_cnt` — FA/CCA/CRC32 counter enum dispatch

## Notes

- **Parallel lane:** complements [W4-92](wave4-92-hal-dm-phydm-igi-iqk-queries.md); unblocks `SetHalODMVar` RX info dump ([W4-96](wave4-96-hal-dm-set-odm-var-rx-dump.md)).
- PHYDM query paths stay FFI-backed; no new MMIO in this slice.

## Acceptance

- L0 + L1 symbol parity for the four exports
- L2 host oracle: fixed `macid_ctl` / `dm_struct` fixtures match C counter and RSSI values

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)