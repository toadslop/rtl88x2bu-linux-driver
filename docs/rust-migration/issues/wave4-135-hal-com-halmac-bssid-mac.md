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

- `rtw_hal_set_bssid` (exported)
- `rtw_hal_set_macaddr_port` and `rtw_hal_get_macaddr_port` (export if currently static under `!CONFIG_MI_WITH_MBSSID_CAM`; thin C wrappers OK until HW_VAR callers move)

## Notes

- **Parallel lane:** uses `rtw_hal_get_port` (W4-48) and `rtw_halmac_set/get_mac_address` / `set_bssid` — no MBSSID CAM overlap.
- Scope is the `#else` branch of `CONFIG_MI_WITH_MBSSID_CAM` and the shared `rtw_hal_set_bssid` body.
- Register fallback for `!RTW_HALMAC` may stay in C.

## Acceptance

- L0 + L1 on 8822B HALMAC build
- L2: trace HALMAC shim calls for port + BSSID/MAC six-byte payloads

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)