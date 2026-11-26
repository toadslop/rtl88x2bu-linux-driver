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

**Out of scope:** `hw_bcn_ctrl_addr` (lines under `!CONFIG_HAS_HW_VAR_BCN_CTRL_ADDR`).
On 8822B, `rtl8822b_get_hwreg` handles `HW_VAR_BCN_CTRL_ADDR` via `port_cfg[].bcn_ctl`
in `hal/rtl8822b/rtl8822b_ops.c`; the `hal_com_rest.c` helper is unused on this profile.
Do not swap the ops helper for the rest.c copy.

## Notes

- **Parallel lane:** disjoint from `hal_com_gpio_wakeup.rs` (W4-123) — new Rust file.
- `CONFIG_RTL8192F` branches delegate to `rtw_hal_set_hwreg` for WOW GPIO; 8822B
  uses the direct `REG_GPIO_PIN_CTRL*` MMIO path in the `#else` block.
- Index > 15: log only, no MMIO (both functions).
- Input path clears direction at `REG_GPIO_PIN_CTRL[+_2] + 2` with `& ~BIT(index)`
  (output sets with `|`). L2 must cover both banks and the invalid index.
- Does not port `CONFIG_GPIO_API` accessors (W4-142).

## Acceptance

- L0 + L1 on 8822B profile with `CONFIG_GPIO_WAKEUP=y`
- L2: output **and input** stub MMIO sequences for index 0–7 and 8–15; invalid index (no MMIO)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)