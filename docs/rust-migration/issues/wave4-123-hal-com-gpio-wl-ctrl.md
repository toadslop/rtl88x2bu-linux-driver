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

- Series check (`IS_8723D/8192F/8822B/8821C/8822C`) and index 13/14 checks are **separate** `if`s; both may call `rtw_hal_set_hwreg(HW_SET_GPIO_WL_CTRL, &enable)` (8822BU can program the same enable twice for GPIO 13/14).

## Notes

- **Parallel lane:** GPIO wakeup — separate Rust submodule from MBSSID / rsvd-page / in-flight RA-PER slices.
- Does not port `rtw_hal_set_output_gpio` / `set_input_gpio` (defer — larger MMIO surface).
- `CONFIG_GPIO_WAKEUP` defaults to off in the Makefile (`CONFIG_GPIO_WAKEUP=y` required for the symbol).

## Acceptance

- L0 with `CONFIG_GPIO_WAKEUP=y` on profile
- L1 symbol parity
- L2: count `rtw_hal_set_hwreg(HW_SET_GPIO_WL_CTRL)` invocations (not only enable bytes); include a non-8822B chip so 8822B-only shortcuts fail

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)