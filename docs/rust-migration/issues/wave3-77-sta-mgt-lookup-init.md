---
title: "[W3-77] stainfo init and hash lookup"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-77
epic: E05
blocked_by: [W3-76]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_sta_mgt.c`](../../../core/rtw_sta_mgt.c) to [`rust/rtw_sta_mgt.rs`](../../../rust/rtw_sta_mgt.rs):

- `_rtw_init_stainfo`
- `rtw_get_stainfo`
- `rtw_get_stainfo_by_offset`
- `rtw_st_ctl_rx`

## Notes

- Adapter-coupled (hash table + locks). W3-37/38/55 covered ACL, AID, st_ctl; alloc/free in W3-78/79.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for stainfo init and hash lookup
