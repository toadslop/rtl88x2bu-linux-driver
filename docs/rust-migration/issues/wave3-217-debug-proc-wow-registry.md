---
title: "[W3-217] Translate rtw_debug_rest.c — proc WoWLAN registry leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-217
epic: E05
blocked_by: []
estimate_loc: 160
---

## Goal

Port WoWLAN registry debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_wow.rs`](../../../rust/rtw_debug_proc_wow.rs) (`CONFIG_WOWLAN`):

- `proc_get_wow_enable` / `proc_set_wow_enable`
- `proc_get_wakeup_event` / `proc_set_wakeup_event`
- `proc_get_wowlan_gpio_info` / `proc_set_wowlan_gpio_info` (registry fields only)

## Notes

- No HAL MMIO in these handlers — registry / pwrctrl field read/write + printk parity.
- `proc_set_pattern_info` and wake-pattern CAM programming deferred to [W3-218](wave3-218-debug-proc-wow-pattern.md).
- Parallel wow lane root.

## Acceptance

- L0 with `CONFIG_WOWLAN`
- L1 on all six handlers
- L2: enable/disable strings; wakeup_event hex line; gpio info get/set vectors
