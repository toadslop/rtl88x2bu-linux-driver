---
title: "[W3-73] Translate rtw_ap.c — STA assoc IE parse (cap/rates/HT/VHT)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-73
epic: E05
blocked_by: [W3-72]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `rtw_ap_parse_sta_capability`
- `rtw_ap_parse_sta_supported_rates`
- `rtw_ap_parse_sta_wmm_ie`
- `rtw_ap_parse_sta_ht_ie`
- `rtw_ap_parse_sta_vht_ie`
- `rtw_ap_parse_sta_multi_ap_ie`

## Notes

- IE → sta_info field population; W3-55 covered TIM/VAPID only.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for STA assoc IE parse helpers
