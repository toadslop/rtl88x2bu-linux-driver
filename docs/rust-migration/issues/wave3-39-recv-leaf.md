---
title: "[W3-39] Translate rtw_recv.c — recv leaf helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-39
epic: E05
blocked_by: [W3-38]
estimate_loc: 200
---

## Goal

Port leaf recv helpers from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to [`rust/rtw_recv.rs`](../../../rust/rtw_recv.rs):

- `rtw_inc_and_chk_continual_no_rx_packet`, `rtw_reset_continual_no_rx_packet`
- `rtw_rframe_del_wfd_ie`

## Notes

- Same continual-error-counter pattern as W3-18 (`rtw_io_rest`).
- Frame validation, reordering, and HAL recv paths stay in C.
- L2: host harness under `tests/host/recv/`.

## Acceptance

- L0 build + L2 host unit tests for recv leaf helpers
