---
title: "[W3-124] ioctl validate and connect/disassociate"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-124
epic: E05
blocked_by: [W3-123]
estimate_loc: 240
---

## Goal

Port helpers from [`core/rtw_ioctl_set.c`](../../../core/rtw_ioctl_set.c) to [`rust/rtw_ioctl_set.rs`](../../../rust/rtw_ioctl_set.rs):

- `rtw_validate_bssid`
- `rtw_validate_ssid`
- `rtw_do_join`
- `rtw_set_802_11_bssid`
- `rtw_set_802_11_ssid`
- `rtw_set_802_11_connect`
- `rtw_set_802_11_disassociate`

## Notes

- Ioctl validation and connect/disassociate leaf helpers; starts tranche 7 on `rtw_ioctl_set.c`.
- Scan/auth/channel setters ship in W3-125.
- Adapter-coupled — expect bindgen + fixtures or thin C shims for L2.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for ioctl validate and connect/disassociate helpers
