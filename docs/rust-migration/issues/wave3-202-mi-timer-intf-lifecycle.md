---
title: "[W3-202] Translate rtw_mi.c — timer cancel and intf lifecycle buddy"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-202
epic: E05
blocked_by: [W3-199]
estimate_loc: 200
---

## Goal

Port the next buddy-process slice from [`core/rtw_mi.c`](../../../core/rtw_mi.c) to [`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `rtw_mi_cancel_all_timer` / `rtw_mi_buddy_cancel_all_timer`
- `rtw_mi_reset_drv_sw` / `rtw_mi_buddy_reset_drv_sw`
- `rtw_mi_intf_start` / `rtw_mi_buddy_intf_start`
- `rtw_mi_intf_stop` / `rtw_mi_buddy_intf_stop`

## Notes

- Continues the **mi** lane after [W3-199](wave3-199-mi-scan-drv-threads.md). Timer cancel and intf start/stop use `_rtw_mi_process`; `reset_drv_sw` buddies call `_rtw_mi_process_without_schk` (see `core/rtw_mi.c`).
- Scan-deny / beacon-update buddy exports remain for a follow-on slice.

## Acceptance

- L0 (`CONFIG_CONCURRENT_MODE`)
- L1 on listed exports
- L2: host JSON — timer/intf buddies invoke `_rtw_mi_process` on expected iface bitmap; `reset_drv_sw` buddies invoke `_rtw_mi_process_without_schk`
