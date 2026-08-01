---
title: "[W3-96] remain-on-channel handlers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-96
epic: E05
blocked_by: [W3-95]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_roch.c`](../../../core/rtw_roch.c) to [`rust/rust/rtw_roch.rs`](../../../rust/rust/rtw_roch.rs):

- `rtw_roch_stay_in_cur_chan`
- `rtw_ro_ch_handler`
- `rtw_cancel_ro_ch_handler`
- `rtw_roch_wk_cmd`
- `rtw_roch_wk_hdl`

## Notes

- CONFIG_IOCTL_CFG80211; gate L2 harness when cfg80211 disabled.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for remain-on-channel handlers
