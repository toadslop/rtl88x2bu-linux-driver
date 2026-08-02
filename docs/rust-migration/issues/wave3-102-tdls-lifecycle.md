---
title: "[W3-102] tdls lifecycle + prohibited checks"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-102
epic: E05
blocked_by: [W3-101]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_tdls.c`](../../../core/rtw_tdls.c) to [`rust/rtw_tdls.rs`](../../../rust/rtw_tdls.rs):

- `check_ap_tdls_prohibited`
- `check_ap_tdls_ch_switching_prohibited`
- `TDLS_check_ch_state`
- `rtw_reset_tdls_info`
- `rtw_init_tdls_info`
- `rtw_free_tdls_info`
- `rtw_is_tdls_enabled`
- `rtw_set_tdls_enable`
- `rtw_tdls_is_setup_allowed`
- `rtw_tdls_is_chsw_allowed`

## Notes

- Low-coupling TDLS lifecycle and policy gate helpers; starts tranche 6 on `rtw_tdls.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for tdls lifecycle and prohibited-check helpers
