---
title: "[W4-142] Translate hal_com_rest.c — CONFIG_GPIO_API accessors"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-142
epic: E06
blocked_by: []
estimate_loc: 150
---

## Goal

Port `CONFIG_GPIO_API` helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_gpio_api.rs`:

- `rtw_hal_get_gpio`
- `rtw_hal_set_gpio_output_value`
- `rtw_hal_config_gpio`

## Notes

- **Parallel lane:** separate from W4-140 (`hal_com_gpio_pin.rs`) — different Kconfig gate.
- Must call `rtw_hal_gpio_func_check`, `LeaveAllPowerSaveModeDirect`, and
  `rtw_ps_deny` / `cancel` in the same order as C.
- `rtw_hal_register_gpio_interrupt` / `disable_gpio_interrupt` are out of scope (defer).

## Acceptance

- L0 + L1 with `CONFIG_GPIO_API=y` on profile
- L2: direction read + output write byte sequence vs C for gpio 3 and gpio 10

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)