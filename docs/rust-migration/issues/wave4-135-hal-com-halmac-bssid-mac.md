---
title: "[W4-135] Translate hal_com_rest.c — HALMAC BSSID and port MAC accessors"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-135
epic: E06
blocked_by: []
estimate_loc: 150
---

## Goal

Port HALMAC identity setters from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_halmac_identity.rs`:

- `rtw_hal_set_bssid` (exported; `HW_VAR_BSSID` — uses `rtw_hal_get_port`, W4-48)
- `rtw_hal_set_macaddr_port` and `rtw_hal_get_macaddr_port` (compiled only when `!CONFIG_MI_WITH_MBSSID_CAM`; absent on pinned 8822B MBSSID profile where `HW_VAR_MAC_ADDR` uses `rtw_hal_set_macaddr_mbid` instead)

## Notes

- **Parallel lane:** HALMAC `set/get_mac_address` and `set_bssid` shims — no MBSSID CAM overlap on pinned profile.
- Port MAC helpers use `get_hw_port(adapter)`, not `rtw_hal_get_port`; only `rtw_hal_set_bssid` uses the W4-48 port helper.
- `rtw_hal_get_macaddr_port` is called from `HW_VAR_GET_MAC_ADDR` only when `!CONFIG_MI_WITH_MBSSID_CAM`.
- Register fallback for `!RTW_HALMAC` may stay in C.

## Acceptance

- L0 + L1 on 8822B HALMAC build
- L2: trace HALMAC shim calls for port + BSSID/MAC six-byte payloads

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)