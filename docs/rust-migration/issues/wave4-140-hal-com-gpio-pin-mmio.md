---
title: "[W4-140] Translate hal_com_rest.c — GPIO pin output/input MMIO"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-140
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Port GPIO MMIO helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_gpio_pin.rs` under `CONFIG_GPIO_WAKEUP`:

- `rtw_hal_set_output_gpio`
- `rtw_hal_set_input_gpio`

Also port `hw_bcn_ctrl_addr` when `!CONFIG_HAS_HW_VAR_BCN_CTRL_ADDR` into the same
module (or `rust/hal_com_bcn_ctrl.rs` if the Makefile swap must stay separate).

## Notes

- **Parallel lane:** disjoint from `hal_com_gpio_wakeup.rs` (W4-123) — new Rust file.
- `CONFIG_RTL8192F` branches delegate to `rtw_hal_set_hwreg` for WOW GPIO; 8822B
  uses the direct `REG_GPIO_PIN_CTRL*` MMIO path in the `#else` block.
- Does not port `CONFIG_GPIO_API` accessors (W4-142).

## Acceptance

- L0 + L1 on 8822B profile with `CONFIG_GPIO_WAKEUP=y`
- L2: host stub records MMIO addr/data writes for index 0–7 and 8–15 output paths

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)