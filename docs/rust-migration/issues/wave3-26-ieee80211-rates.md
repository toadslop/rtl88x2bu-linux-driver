---
title: "[W3-26] Translate rtw_ieee80211.c — rate and network type helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-26
epic: E05
blocked_by: [W3-25]
estimate_loc: 200
---

## Goal

Port rate classification helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_get_bit_value_from_ieee_value`, `rtw_check_network_type`
- `rtw_set_supported_rate`, `rtw_filter_suppport_rateie`, `rtw_update_rate_bymode`

## Notes

- W3-03 ported IE parse getters; this starts the `*_rest.c` split for remaining ieee80211 logic.
- L2: extend `tests/host/ie/` with rate/network-type oracle vectors.

## Acceptance

- L0 build + L2 host unit tests for rate helpers
