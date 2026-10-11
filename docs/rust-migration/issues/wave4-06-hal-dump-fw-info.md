---
title: "[W4-06] Translate hal_com.c — rtw_dump_fw_info formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-06
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `rtw_dump_fw_info` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) to
[`rust/hal_com.rs`](../../../rust/hal_com.rs).

## Notes

- Prints firmware major/minor when `bFWReady`, else a fixed “FW not ready” line —
  no USB/register access beyond reading `HAL_DATA_TYPE` fields already in memory.
- Use the same `RTW_PRINT_SEL` / `dump_sel` pattern as W4-03/W4-04.

## Acceptance

- L0 + L1
- L2 golden strings for ready vs not-ready fixture `HAL_DATA_TYPE` blobs
