---
title: "[W3-93] traffic/LPS dynamic watchdog"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-93
epic: E05
blocked_by: [W3-92]
estimate_loc: 415
---

## Goal

Port helpers from [`core/rtw_cmd.c`](../../../core/rtw_cmd.c) to [`rust/rtw_cmd_rest.rs`](../../../rust/rtw_cmd_rest.rs):

- `traffic_status_watchdog` (~138 LOC)
- `_lps_chk_by_tp` (~92 LOC)
- `_lps_chk_by_pkt_cnts` (~100 LOC)
- `lps_ctrl_wk_hdl` (~87 LOC)

## Notes

- **Multi-PR slice (~415 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each).
- Links cmd workqueue to power-save policy; pairs with W3-94 pwrctrl slice.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for traffic/LPS dynamic watchdog
