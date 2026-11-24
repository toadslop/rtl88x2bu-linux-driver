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

- **Parallel lane:** HALMAC `rtw_halmac_get/set_network_type` on pinned 8822B profile (`RTW_HALMAC`); both get/set use `adapter->hw_port` (not `rtw_hal_get_port`).
- Non-HALMAC `set_msr` is a switch on `adapter->hw_port` (MSR / MSR1 RMW per port); it does not call `rtw_halmac_set_network_type` and may remain in C for this slice.
- `rtw_hal_net_type_decision` (MBSSID CAM): on HW_PORT0 with MBID CAM entries, non-`_HW_STATE_NOLINK_` becomes `_HW_STATE_AP_`. Under `RTW_HALMAC`, the helper runs once before `rtw_halmac_set_network_type` for `adapter->hw_port`. Under `!RTW_HALMAC`, it runs only inside the `HW_PORT0` `MSR` RMW branch (other ports use raw `net_type`; `MSR1` ports are `CONFIG_RTL8814A` only).

## Acceptance

- L0 + L1 on HALMAC profile
- L2: stub records `hw_port` + `net_type` bytes passed to HALMAC shim for get/set pairs

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)