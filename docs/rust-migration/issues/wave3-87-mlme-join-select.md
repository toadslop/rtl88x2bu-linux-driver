---
title: "[W3-87] join candidate select"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-87
epic: E05
blocked_by: [W3-86]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_select_and_join_from_scanned_queue`
- `rtw_check_join_candidate` (static in C)
- `_rtw_sitesurvey_condition_check`

## Notes

- FSM entry before join_cmd_hdl; W3-53..67 covered leaf mlme helpers.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).
- **Host oracle subset (PR1):** under `HOST_MLME_JOIN_SELECT_TEST`, the C/Rust oracles intentionally omit production-only paths (DFS non-OCP, L2 roam age/ESS filters, `roam_network` shortcut, `WIFI_ASOC_STATE` disassoc, WOWLAN/antenna hooks). L2 green on current vectors does not imply full kernel parity — extend vectors before the kernel Rust swap.

## Acceptance

- L0 build + L2 host unit tests for join candidate select
