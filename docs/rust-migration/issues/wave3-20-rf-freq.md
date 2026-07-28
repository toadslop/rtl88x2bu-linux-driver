---
title: "[W3-20] Translate rtw_rf.c — channel/frequency conversion"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-20
epic: E05
blocked_by: [W3-19]
estimate_loc: 200
---

## Goal

Port frequency conversion helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `rtw_ch2freq`, `rtw_freq2ch`
- `rtw_chbw_to_freq_range`

## Notes

- Builds on W3-19 (`rtw_get_center_ch` dependency for `rtw_chbw_to_freq_range`).
- Same `core/rtw_rf_rest.c` / `rust/rtw_rf_rest.rs` split as W3-19.
- L2: extend `tests/host/rf/` with freq-range oracle vectors (2.4G/5G, 20/40/80/160 MHz).

## Acceptance

- L0 build + L2 host unit tests for freq conversion helpers
