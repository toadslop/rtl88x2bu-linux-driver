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

- `_rtw_IOL_append_WB_cmd`, `_rtw_IOL_append_WW_cmd`, `_rtw_IOL_append_WD_cmd`,
  `_rtw_IOL_append_WRF_cmd` (masked + simple variants per `#ifdef CONFIG_IOL_NEW_GENERATION`)
- `rtw_IOL_append_DELAY_US_cmd`, `rtw_IOL_append_DELAY_MS_cmd`, `rtw_IOL_append_END_cmd`, `rtw_IOL_append_LLT_cmd`
- `rtw_IOL_cmd_boundary_handle`, `rtw_IOL_append_cmds`

## Notes

- Buffer layout only; no HAL exec/sync paths (`rtw_IOL_exec_*` stay in C).
- **`CONFIG_IOL_NEW_GENERATION` fork:** the new branch uses `struct ioreg_cfg` with optional
  mask fields (8- or 12-byte cmd length); the legacy branch uses `IOL_CMD` (8-byte fixed).
  This 8822BU driver does **not** define `CONFIG_IOL_NEW_GENERATION` in
  [`include/autoconf.h`](../../../include/autoconf.h) — target the legacy byte-stream oracle
  unless the build config changes.
- Extract to `rtw_iol_rest.c` if needed for L1 symbol checks.
- L2: new `tests/host/iol/` — append → expected byte stream with minimal xmit_frame stub.

## Acceptance

- L0 build + L2 host unit tests for IOL command-buffer append encoders
