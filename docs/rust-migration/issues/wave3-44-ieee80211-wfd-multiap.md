---
title: "[W3-44] Translate rtw_ieee80211.c — WFD and multi-AP IE helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-44
epic: E05
blocked_by: [W3-43]
estimate_loc: 200
---

## Goal

Port WFD and multi-AP IE helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_del_wfd_ie`, `rtw_bss_ex_del_wfd_ie`, `rtw_del_wfd_attr`, `rtw_bss_ex_del_wfd_attr`
- `rtw_get_multi_ap_ie_ext`

## Notes

- Complements W3-39 `rtw_rframe_del_wfd_ie` (recv path); these operate on bss/IE buffers.
- Debug dump printers stay in C.
- L2: extend `tests/host/ie/` with WFD attr and multi-AP ext nibble cases.

## Acceptance

- L0 build + L2 host unit tests for WFD and multi-AP IE helpers
