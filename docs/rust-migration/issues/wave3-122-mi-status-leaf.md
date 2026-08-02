---
title: "[W3-122] mi status and check_status leaf"
labels: [rust-migration, phase-1, wave-3, size/~250]
type: child
id: W3-122
epic: E05
blocked_by: [W3-121]
estimate_loc: 220
---

## Goal

Port helpers from [`core/rtw_mi.c`](../../../core/rtw_mi.c) to [`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `rtw_mi_status_by_ifbmp`
- `rtw_mi_status`
- `rtw_mi_status_no_self`
- `rtw_mi_status_no_others`
- `rtw_mi_status_merge`
- `dump_mi_status`
- `dump_dvobj_mi_status`
- `rtw_mi_update_iface_status`
- `rtw_mi_check_status`

## Notes

- Multi-interface status aggregation and dump helpers.
- Netif queue/carrier buddy helpers ship in W3-123.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mi status and check_status leaf helpers
