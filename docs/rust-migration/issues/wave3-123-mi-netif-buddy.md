---
title: "[W3-123] mi netif buddy queue/carrier leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-123
epic: E05
blocked_by: [W3-122]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mi.c`](../../../core/rtw_mi.c) to [`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `rtw_mi_netif_caroff_qstop`
- `rtw_mi_buddy_netif_caroff_qstop`
- `rtw_mi_netif_caron_qstart`
- `rtw_mi_buddy_netif_caron_qstart`
- `rtw_mi_netif_stop_queue`
- `rtw_mi_buddy_netif_stop_queue`
- `rtw_mi_netif_wake_queue`
- `rtw_mi_buddy_netif_wake_queue`
- `rtw_mi_netif_carrier_on`
- `rtw_mi_buddy_netif_carrier_on`
- `rtw_mi_netif_carrier_off`
- `rtw_mi_buddy_netif_carrier_off`

## Notes

- Multi-interface netif queue/carrier buddy helpers; covers the listed exported
  netif buddy surface in this slice.
- Remaining `rtw_mi.c` exports (e.g. `rtw_mi_scan_abort`, `rtw_mi_start_drv_threads`,
  buddy thread/timer paths) stay in C until a later tranche.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mi netif buddy queue/carrier leaf helpers
