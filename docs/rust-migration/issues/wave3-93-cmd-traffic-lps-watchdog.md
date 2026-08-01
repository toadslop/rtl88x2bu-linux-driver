---
title: "[W3-93] traffic/LPS dynamic watchdog"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-93
epic: E05
blocked_by: [W3-92]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rust/rtw_cmd_rest.rs`](../../../rust/rust/rtw_cmd_rest.rs):

- `traffic_status_watchdog`
- `_lps_chk_by_tp`
- `_lps_chk_by_pkt_cnts`
- `lps_ctrl_wk_hdl`

## Notes

- Links cmd workqueue to power-save policy; pairs with W3-94 pwrctrl slice.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for traffic/LPS dynamic watchdog
