---
title: "[W4-123] Translate hal_com_rest.c — GPIO WL control switch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-123
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Port `rtw_hal_switch_gpio_wl_ctrl` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_gpio_wakeup.rs` under `CONFIG_GPIO_WAKEUP`:

- 8822B series branch and GPIO 13/14 special case call `rtw_hal_set_hwreg(HW_SET_GPIO_WL_CTRL, …)` like C.

## Notes

- **Parallel lane:** GPIO wakeup — separate Rust submodule from MBSSID / rsvd-page / in-flight RA-PER slices.
- Does not port `rtw_hal_set_output_gpio` / `set_input_gpio` (defer — larger MMIO surface).

## Acceptance

- L0 with `CONFIG_GPIO_WAKEUP` enabled on profile
- L1 symbol parity
- L2: trace `rtw_hal_set_hwreg` enable bytes for index/series matrix

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)