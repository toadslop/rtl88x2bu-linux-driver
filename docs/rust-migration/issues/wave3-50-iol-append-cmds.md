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

Port IOL append/encode helpers from [`core/rtw_iol.c`](../../../core/rtw_iol.c) to [`rust/rtw_iol_rest.rs`](../../../rust/rtw_iol_rest.rs).

**8822BU default config (legacy branch — no `CONFIG_IOL_NEW_GENERATION`):**

- `_rtw_IOL_append_WB_cmd`, `_rtw_IOL_append_WW_cmd`, `_rtw_IOL_append_WD_cmd` (simple, no mask)
- `rtw_IOL_append_DELAY_US_cmd`, `rtw_IOL_append_DELAY_MS_cmd`, `rtw_IOL_append_END_cmd`, `rtw_IOL_append_LLT_cmd`
- `rtw_IOL_append_cmds`

**Out of scope unless `CONFIG_IOL_NEW_GENERATION` is enabled:** `_rtw_IOL_append_WRF_cmd` (masked
variants), `rtw_IOL_cmd_boundary_handle` (legacy uses `rtw_IOL_exec_cmd_array_sync` instead).

## Notes

- Buffer layout only; no HAL exec/sync paths (`rtw_IOL_exec_*` stay in C).
- **`CONFIG_IOL` gate:** the entire [`core/rtw_iol.c`](../../../core/rtw_iol.c) body is wrapped in
  `#ifdef CONFIG_IOL`, which is **not** defined in
  [`include/autoconf.h`](../../../include/autoconf.h) for this 8822BU driver. L0/L1 for this slice
  may require enabling `CONFIG_IOL` in the build config or stubbing the gated C objects — note in the
  PR if the Makefile link set changes.
- **`CONFIG_IOL_NEW_GENERATION` fork:** the new branch uses `struct ioreg_cfg` with optional mask
  fields (8- or 12-byte cmd length); the legacy branch uses `IOL_CMD` (8-byte fixed). This driver
  targets the **legacy** byte-stream oracle unless the build config changes.
- Extract to `rtw_iol_rest.c` if needed for L1 symbol checks.
- L2: new `tests/host/iol/` — append → expected byte stream with minimal xmit_frame stub.

## Acceptance

- L0 build + L2 host unit tests for IOL command-buffer append encoders
