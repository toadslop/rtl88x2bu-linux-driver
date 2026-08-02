---
title: "[W3-127] btcoex handler and AMPDU policy leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-127
epic: E05
blocked_by: [W3-126]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_btcoex.c`](../../../core/rtw_btcoex.c) to [`rust/rtw_btcoex.rs`](../../../rust/rtw_btcoex.rs):

- `rtw_btcoex_Handler`
- `rtw_btcoex_IsBTCoexRejectAMPDU`
- `rtw_btcoex_IsBTCoexCtrlAMPDUSize`
- `rtw_btcoex_GetAMPDUSize`
- `rtw_btcoex_SetManualControl`
- `rtw_btcoex_set_policy_control`
- `rtw_btcoex_IsBtDisabled`
- `rtw_btcoex_Switch`

## Notes

- BT coexistence handler and AMPDU/policy leaf helpers.
- `rtw_btcoex_wifionly.c` (tiny) can fold into a later tranche if needed.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for btcoex handler and AMPDU policy leaf helpers
