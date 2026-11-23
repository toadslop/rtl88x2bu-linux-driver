---
title: "[W4-139] Translate hal_com_rest.c — FW media status report H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-139
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port media-status H2C cluster from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_fw_media.rs`:

- `rtw_hal_set_FwMediaStatusRpt_cmd`
- `rtw_hal_set_FwMediaStatusRpt_single_cmd`
- `rtw_hal_set_FwMediaStatusRpt_range_cmd`

## Notes

- **Parallel lane:** H2C + `macid_ctl` updates on disconnect (`bw`, `vht_en`, rate bmp clears).
- Chip-specific post-H2C blocks (`RTL8188E`, `RTL8812A`, `Hal_PatchwithJaguar_8812`) remain in C behind existing `#if defined` gates — 8822B path only in Rust.
- `CONFIG_FW_MULTI_PORT_SUPPORT` adds port num to parm; `CONFIG_TX_AC_LIFETIME` may call `rtw_hal_update_tx_aclt` when `op_num_change_bmp` set (delegate to W4-124 Rust/C).
- `CONFIG_RTL8822C` DTP hook stays C unless a separate slice lands.

## Acceptance

- L0 + L1 on 8822B HALMAC profile
- L2: H2C parm bytes + `rtw_macid_ctl_set_h2c_msr` side effects for single-macid connect/disconnect vectors

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)