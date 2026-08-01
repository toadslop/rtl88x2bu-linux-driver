---
title: "[W3-46] Translate rtw_recv.c — LLC parse, ethhdr, and BMC gate"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-46
epic: E05
blocked_by: [W3-45]
estimate_loc: 200
---

## Goal

Port recv LLC/ethhdr helpers from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to [`rust/rtw_recv.rs`](../../../rust/rtw_recv.rs):

- `rtw_recv_llc_parse`, `wlanhdr_to_ethhdr`, `adapter_allow_bmc_data_rx`

## Notes

- Builds on W3-39 recv leaf helpers; mutates `union recv_frame` and reads adapter mode bits.
- Frame validation, reordering, and HAL recv paths stay in C.
- L2: extend `tests/host/recv/` with MSDU SNAP cases and populated recv_frame/adapter fixtures.

## Acceptance

- L0 build + L2 host unit tests for LLC parse, ethhdr conversion, and BMC gate
