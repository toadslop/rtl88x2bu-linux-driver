---
title: "[W4-132] Translate hal_com_rest.c — SW TXBCN interval adjust (cancelled)"
labels: [rust-migration, phase-1, wave-4, size/~200, cancelled]
type: child
id: W4-132
epic: E06
blocked_by: []
estimate_loc: 40
status: cancelled
---

## Status (cancelled)

`rtw_hal_bcn_interval_adjust` lives under `#ifdef CONFIG_SWTIMER_BASED_TXBCN`, but the
pinned concurrent profile (`CONFIG_IFACE_NUMBER=3`, `CONFIG_AP_MODE`,
`CONFIG_HWMPCAP_GEN2` / 8822B) enables `CONFIG_FW_HANDLE_TXBCN` and **undefs**
`CONFIG_SWTIMER_BASED_TXBCN` in [`include/drv_conf.h`](../../../include/drv_conf.h).
The symbol is not compiled into `88x2bu.ko` for default L0/L1 gates — do not implement
this slice on the standard migration profile.

GitHub #2256 is closed as not applicable. Re-open only if a tracked L0 matrix config
keeps `CONFIG_SWTIMER_BASED_TXBCN=y` and needs the port.

## Original goal (reference only)

Port `rtw_hal_bcn_interval_adjust` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c).