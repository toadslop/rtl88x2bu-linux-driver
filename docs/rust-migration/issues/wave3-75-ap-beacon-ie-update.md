---
title: "[W3-75] Translate rtw_ap.c — beacon TIM and generic IE update"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-75
epic: E05
blocked_by: [W3-74]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `update_BCNTIM`
- `rtw_add_bcn_ie`
- `rtw_remove_bcn_ie`

## Notes

- Beacon IE byte helpers; rtw_set_tim_ie is in W3-55.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for beacon TIM/generic IE update
