---
title: "[W3-81] beacon HT/WPS/ERP refresh"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-81
epic: E05
blocked_by: [W3-80]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `update_bcn_erpinfo_ie`
- `update_bcn_htinfo_ie`
- `update_bcn_wps_ie`
- `update_bcn_vendor_spec_ie`
- `_update_beacon`

## Notes

- CONFIG_AP_MODE beacon IE refresh; W3-75 covered TIM/generic add/remove.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for beacon HT/WPS/ERP refresh
