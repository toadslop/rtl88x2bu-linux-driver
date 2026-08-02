---
title: "[W3-110] ft IE update/build leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-110
epic: E05
blocked_by: [W3-109]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ft.c`](../../../core/rtw_ft.c) to [`rust/rtw_ft.rs`](../../../rust/rtw_ft.rs):

- `rtw_ft_info_init`
- `rtw_ft_chk_roaming_candidate`
- `rtw_ft_update_rsnie`
- `rtw_ft_update_mdie`
- `rtw_ft_update_ftie`
- `rtw_ft_build_auth_req_ies`
- `rtw_ft_build_assoc_req_ies`
- `rtw_ft_update_auth_rsp_ies`

## Notes

- Fast Transition IE update/build leaf helpers; medium adapter coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for ft IE update/build leaf helpers
