---
title: "[W4-134] Translate hal_com_rest.c — MSR network type get and set"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-134
epic: E06
blocked_by: []
estimate_loc: 180
---

## Goal

Port MSR helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
new `rust/hal_com_msr.rs`:

- `rtw_hal_get_msr` (export for HW_VAR paths)
- `rtw_hal_set_msr`
- `rtw_hal_net_type_decision` when `CONFIG_MI_WITH_MBSSID_CAM` && `CONFIG_MBSSID_CAM`

## Notes

- **Parallel lane:** HALMAC `rtw_halmac_get/set_network_type` on pinned 8822B profile (`RTW_HALMAC`).
- Non-HALMAC register bitbang may remain in C behind `!RTW_HALMAC` for this slice.
- `set_msr` applies `net_type_decision` only on HW_PORT0 when MBID CAM entries exist.

## Acceptance

- L0 + L1 on HALMAC profile
- L2: stub records `hw_port` + `net_type` bytes passed to HALMAC shim for get/set pairs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)