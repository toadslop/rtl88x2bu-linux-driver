---
title: "[W4-75] Translate hal_intf.c — hal_deinit teardown"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-75
epic: E06
blocked_by: [W4-74]
estimate_loc: 60
---

## Goal

Port `rtw_hal_deinit` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `padapter->hal_func.hal_deinit` dispatch
- Success path: `rtw_led_control(LED_CTL_POWER_OFF)` + `rtw_set_hw_init_completed(FALSE)`
- Failure path: `RTW_INFO` log string identical to C

## Notes

- Does not port `rtw_hal_init` (large PHYDM tail) in this slice.
- Complements init/configure issues W4-59–W4-72 in the hal_intf lane.

## Acceptance

- L0 + L1 for `rtw_hal_deinit`
- L3 insmod/rmmod smoke (no new Oops)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-74