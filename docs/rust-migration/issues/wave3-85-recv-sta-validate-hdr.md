---
title: "[W3-85] sta rx validate and stats"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-85
epic: E05
blocked_by: [W3-84]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to [`rust/rust/rtw_recv.rs`](../../../rust/rust/rtw_recv.rs):

- `count_rx_stats`
- `rtw_sta_rx_data_validate_hdr`

## Notes

- W3-39/46/47 covered leaf/LLC/PN; datapath validation stays adapter-coupled.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sta rx validate and stats
