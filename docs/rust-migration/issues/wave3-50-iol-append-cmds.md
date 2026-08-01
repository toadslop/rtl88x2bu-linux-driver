---
title: "[W3-50] Translate rtw_iol.c — IOL command-buffer append encoders"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-50
epic: E05
blocked_by: [W3-49]
estimate_loc: 200
---

## Goal

Port IOL append/encode helpers from [`core/rtw_iol.c`](../../../core/rtw_iol.c) to [`rust/rtw_iol_rest.rs`](../../../rust/rtw_iol_rest.rs):

- `_rtw_IOL_append_WB_cmd`, `_WW_cmd`, `_WD_cmd`, `_WRF_cmd` (masked + simple variants)
- `rtw_IOL_append_DELAY_US_cmd`, `rtw_IOL_append_DELAY_MS_cmd`, `rtw_IOL_append_END_cmd`, `rtw_IOL_append_LLT_cmd`
- `rtw_IOL_cmd_boundary_handle`, `rtw_IOL_append_cmds`

## Notes

- Buffer layout only; no HAL exec/sync paths (`rtw_IOL_exec_*` stay in C).
- Extract to `rtw_iol_rest.c` if needed for L1 symbol checks.
- L2: new `tests/host/iol/` — append → expected byte stream with minimal xmit_frame stub.

## Acceptance

- L0 build + L2 host unit tests for IOL command-buffer append encoders
