---
title: "[W4-156] Translate hal_com_rest.c — LPS PG info rsvd page build/download"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-156
epic: E06
blocked_by: []
estimate_loc: 130
---

## Goal

Under `CONFIG_LPS_PG`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_lps_pg_info.rs`:

- `rtw_hal_build_lps_pg_info_rsvd_page`
- `rtw_hal_set_lps_pg_info_rsvd_page`

## Notes

- **Parallel lane:** 16-byte `LPSPG` info block (macid, MBSSID CAM, pattern cam, sec CAM
  ids, drv rsvd page count) + `rsvd_page_cache_update_data` / `rtw_halmac_download_rsvd_page`.
- Optional `CONFIG_MBSSID_CAM` / `CONFIG_WOWLAN` / `CONFIG_BEAMFORMING` byte slots — preserve guards.
- When `CONFIG_LPS_PG` is off, compile stub exports for L0 link parity.

## Acceptance

- L0 + L1
- L2: golden 16-byte buffer for STA iface with seeded macid and sec CAM list

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)