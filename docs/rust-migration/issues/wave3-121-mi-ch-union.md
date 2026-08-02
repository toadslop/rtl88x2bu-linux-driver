---
title: "[W3-121] mi channel union and stay-in checks"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-121
epic: E05
blocked_by: [W3-120]
estimate_loc: 210
---

## Goal

Port helpers from [`core/rtw_mi.c`](../../../core/rtw_mi.c) to [`rust/rtw_mi.rs`](../../../rust/rtw_mi.rs):

- `rtw_mi_update_union_chan_inf`
- `rtw_mi_stayin_union_ch_chk`
- `rtw_mi_stayin_union_band_chk`
- `rtw_mi_get_ch_setting_union_by_ifbmp`
- `rtw_mi_get_ch_setting_union`
- `rtw_mi_get_ch_setting_union_no_self`

## Notes

- Multi-interface channel union helpers; starts tranche 7 on `rtw_mi.c`.
- Netif and status helpers ship in W3-122/W3-123.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for mi channel union and stay-in check helpers
