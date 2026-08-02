---
title: "[W3-108] wnm BTM parse/reset leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-108
epic: E05
blocked_by: [W3-107]
estimate_loc: 210
---

## Goal

Port helpers from [`core/rtw_wnm.c`](../../../core/rtw_wnm.c) to [`rust/rtw_wnm.rs`](../../../rust/rtw_wnm.rs):

- `rtw_wnm_btm_req_hdr_parsing`
- `rtw_wnm_btm_candidates_offset_get`
- `rtw_wnm_btm_candidate_validity`
- `rtw_wnm_btm_rsp_candidates_sz_get`
- `rtw_wnm_reset_btm_candidate`
- `rtw_wnm_reset_btm_cache`
- `rtw_wnm_reset_btm_state`
- `rtw_wnm_nb_elem_parsing`

## Notes

- WNM BTM parse/reset leaf helpers; low-medium coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for wnm BTM parse/reset leaf helpers
