---
title: "[W4-62] Translate hal_intf.c — hal_data_deinit + hal_free_data"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-62
epic: E06
blocked_by: [W4-61]
estimate_loc: 200
---

## Goal

Port HAL data teardown from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c):

- `rtw_hal_data_deinit`
- `rtw_hal_free_data`

## Notes

- Must mirror allocation order from W4-61 `rtw_hal_data_init` tranche(s).
- NULL-safe free paths must match C (no extra early returns that leak).

## Acceptance

- L0 + L1 symbol parity
- L3: clean `rmmod` after successful `insmod` in QEMU (no kmem leak WARN from this path)
