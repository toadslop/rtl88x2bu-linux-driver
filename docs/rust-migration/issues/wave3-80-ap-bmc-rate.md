---
title: "[W3-80] BMC multicast tx rate helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-80
epic: E05
blocked_by: [W3-79]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rust/rtw_ap_rest.rs`](../../../rust/rust/rtw_ap_rest.rs):

- `rtw_ap_find_bmc_rate`
- `rtw_ap_find_mini_tx_rate`
- `rtw_update_bmc_sta_tx_rate`
- `rtw_init_bmc_sta_tx_rate`
- `update_bmc_sta`

## Notes

- Mixed: `rtw_ap_find_bmc_rate` is table-driven/pure; remainder touches asoc_list under AP mode.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for BMC multicast tx rate helpers
