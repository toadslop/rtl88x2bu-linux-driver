---
title: "[W3-54] Translate rtw_mlme_ext.c — chset non-OCP and search/valid"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-54
epic: E05
blocked_by: [W3-53]
estimate_loc: 200
---

## Goal

Port chset non-OCP and search helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `rtw_chset_is_chbw_non_ocp`, `rtw_chset_is_ch_non_ocp`, `rtw_chset_get_ch_non_ocp_ms`
- `_rtw_chset_update_non_ocp`, `rtw_chset_update_non_ocp`, `rtw_chset_update_non_ocp_ms`
- `rtw_chset_search_ch`, `rtw_chset_is_chbw_valid`, `rtw_chset_sync_chbw`

## Notes

- Depends on W3-19/W3-20 freq helpers already on master.
- Mgmt frame handlers and `issue_*` paths stay in C (~16k LOC remainder).
- L2: extend T4 chplan harness or new `tests/host/chset/` with synthetic channel sets.

## Acceptance

- L0 build + L2 host unit tests for chset non-OCP and search/valid helpers
