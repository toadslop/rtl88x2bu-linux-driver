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
- **Power-save / func-check order** (match C exactly in all three exports):
  1. `rtw_hal_gpio_func_check` — on `_FAIL`, return immediately (`0` from get, `-1` from set/config); **no** `rtw_ps_deny`.
  2. `rtw_ps_deny(PS_DENY_IOCTL)`.
  3. `LeaveAllPowerSaveModeDirect` (`rtw_hal_get_gpio` logs `rf_pwrstate` between 2 and 3).
  4. **`rtw_hal_config_gpio` only:** `rtw_hal_gpio_multi_func_reset` before direction RMW.
  5. `rtw_ps_deny_cancel` on the success path.
- `rtw_hal_register_gpio_interrupt` / `disable_gpio_interrupt` are out of scope (defer).

## Acceptance

- L0 + L1 with `CONFIG_GPIO_API=y` on profile
- L2: gpio 3 and gpio 10 oracles for **get, set, and config** (func-check early return,
  `gpio_multi_func_reset` on config, direction/output byte sequence)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)