---
title: "[W4-118] Translate hal_com_rest.c — H2C request PER report cmd"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-118
epic: E06
blocked_by: []
estimate_loc: 80
---

## Goal

Port `rtw_hal_set_req_per_rpt_cmd` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs) under `RTW_PER_CMD_SUPPORT_FW`:

- Pack `group_macid`, `rpt_type`, and `macid_bitmap` into the 5-byte H2C buffer
  with the same LE bit-field layout as C.
- Call `rtw_hal_fill_h2c_cmd(adapter, H2C_REQ_PER_RPT, …)`.

## Notes

- **Parallel lane:** H2C packing only — no overlap with `hal_com_c2h_status.rs` ([W4-58](wave4-58-hal-com-c2h-status.md)) or in-flight `hal_dm` work.
- `CONFIG_MI_WITH_MBSSID_CAM` / core `rtw_cmd.c` callers unchanged; this slice is the HAL helper only.

## Acceptance

- L0 build with `RTW_PER_CMD_SUPPORT_FW` enabled on the pinned 8822BU profile
- L1 symbol parity for `rtw_hal_set_req_per_rpt_cmd`
- L2: host vectors compare packed H2C bytes and `rtw_hal_fill_h2c_cmd` args to C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)