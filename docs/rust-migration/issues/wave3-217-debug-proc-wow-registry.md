---
title: "[W3-217] Translate rtw_debug_rest.c — proc WoWLAN registry leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-217
epic: E05
blocked_by: []
estimate_loc: 190
---

## Goal

Port WoWLAN registry debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_wow.rs`](../../../rust/rtw_debug_proc_wow.rs):

**`CONFIG_WOWLAN`**

- `proc_get_wow_enable` / `proc_set_wow_enable`
- `proc_get_wakeup_event` / `proc_set_wakeup_event`
- `proc_get_wakeup_reason` (read `pwrctrl_priv.wowlan_last_wake_reason`)

**`CONFIG_GPIO_WAKEUP`** (same Rust module; separate Kconfig gate in C)

- `proc_get_wowlan_gpio_info` / `proc_set_wowlan_gpio_info`

## Notes

- Wow enable / wakeup event / wake reason: registry or pwrctrl field access only — no HAL calls.
- GPIO setter leaves PS, then calls `rtw_hal_set_output_gpio` / `rtw_hal_set_input_gpio` (or
  `rtw_hal_switch_gpio_wl_ctrl`) per `CONFIG_WAKEUP_GPIO_INPUT_MODE` — stub HAL for L2 or gate
  L2 to field updates without GPIO programming.
- `proc_set_pattern_info` and wake-pattern CAM programming deferred to [W3-218](wave3-218-debug-proc-wow-pattern.md).
- Parallel wow lane root.

## Acceptance

- L0 with `CONFIG_WOWLAN` (+ GPIO symbols when testing GPIO procs)
- L1 on all seven symbols (five wow + two gpio)
- L2: enable/disable strings; wakeup_event hex line; wake-reason line; gpio get/set field vectors
