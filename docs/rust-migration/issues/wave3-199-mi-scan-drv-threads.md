---
title: "[W3-199] Translate rtw_mi.c — scan abort and drv thread control"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-199
epic: E05
blocked_by: [W3-198]
estimate_loc: 200
---

## Goal

Port buddy-process helpers deferred from [W3-123](wave3-123-mi-netif-buddy.md) in [`core/rtw_mi.c`](../../../core/rtw_mi.c) to
[`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `rtw_mi_scan_abort` / `rtw_mi_buddy_scan_abort`
- `_rtw_mi_start_drv_threads` / `_rtw_mi_stop_drv_threads` and exported `rtw_mi_stop_drv_threads` / `rtw_mi_buddy_stop_drv_threads`

## Notes

- Reuses `_rtw_mi_process` / `_rtw_mi_process_without_schk` already in C (FFI) — same pattern as W3-123 netif buddy slice.
- Same **mi** lane as [W3-198](wave3-198-mi-dump-status.md); chains on dump completion to limit `rtw_mi.rs` merge collisions.

## Acceptance

- L0 build (`CONFIG_CONCURRENT_MODE`)
- L1 on listed exports
- L2: host vectors — scan abort propagates to stub iface bitmap; start/stop thread hooks invoked per buddy rules
