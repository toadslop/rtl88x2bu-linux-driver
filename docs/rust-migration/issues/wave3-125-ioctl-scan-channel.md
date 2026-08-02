---
title: "[W3-125] ioctl scan/auth/channel setters"
labels: [rust-migration, phase-1, wave-3, size/~250]
type: child
id: W3-125
epic: E05
blocked_by: [W3-124]
estimate_loc: 230
---

## Goal

Port helpers from [`core/rtw_ioctl_set.c`](../../../core/rtw_ioctl_set.c) to [`rust/rtw_ioctl_set.rs`](../../../rust/rtw_ioctl_set.rs):

- `rtw_set_802_11_infrastructure_mode`
- `rtw_set_802_11_bssid_list_scan`
- `rtw_set_acs_sitesurvey`
- `rtw_set_802_11_authentication_mode`
- `rtw_set_802_11_add_wep`
- `rtw_get_cur_max_rate`
- `rtw_set_scan_mode`
- `rtw_set_channel_plan`
- `rtw_set_country`
- `rtw_set_band`

## Notes

- Ioctl scan/auth/rate/channel setter leaf helpers.
- `rtw_ioctl_query.c` (tiny) can fold into a later tranche if needed.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for ioctl scan/auth/channel setter helpers
