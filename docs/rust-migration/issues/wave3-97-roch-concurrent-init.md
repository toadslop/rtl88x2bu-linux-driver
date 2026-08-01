---
title: "[W3-97] concurrent roch and init"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-97
epic: E05
blocked_by: [W3-96]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_roch.c`](../../../core/rtw_roch.c) to [`rust/rust/rtw_roch.rs`](../../../rust/rust/rtw_roch.rs):

- `rtw_concurrent_handler`
- `chk_need_stay_in_cur_chan`
- `get_remain_ch`
- `rtw_init_roch_info`
- `rtw_ap_roch_ch_switch_timer_process`

## Notes

- Concurrent-mode roch helpers; completes tranche 5 roch coverage.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for concurrent roch and init
