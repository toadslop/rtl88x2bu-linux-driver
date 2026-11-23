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

- **Parallel lane:** H2C first; `macid_ctl` side effects run only when `rtw_hal_fill_h2c_cmd` returns `_SUCCESS`.
- After success, `SET_H2CCMD_MSRRPT_PARM_MACID_IND(parm, 0)` clears the cached MSR byte’s MACID_IND bit; when `macid_ind == 0`, set `macid_end = macid` so `single_cmd` (initial `macid_end == 0`) still updates `macid_ctl` for `macid > 0`.
- Loop `macid`…`macid_end`: `rtw_macid_ctl_set_h2c_msr`, and on disconnect `bw` / `vht_en` / rate-bmp clears.
- `rtw_hal_update_tx_aclt` (W4-124) runs only when `op_num_change_bmp` is non-zero; `rtw_update_tx_rate_bmp` on disconnect only after H2C success.
- Chip-specific post-H2C blocks (`RTL8188E`, `RTL8812A`, `Hal_PatchwithJaguar_8812`) remain in C behind existing `#if defined` gates — 8822B path only in Rust.
- `CONFIG_FW_MULTI_PORT_SUPPORT`: `SET_H2CCMD_MSRRPT_PARM_PORT_NUM` uses `rtw_hal_get_port(adapter)` (W4-48).
- `CONFIG_TX_AC_LIFETIME` may call `rtw_hal_update_tx_aclt` when `op_num_change_bmp` set (delegate to W4-124 Rust/C).
- `CONFIG_RTL8822C` DTP hook stays C unless a separate slice lands.

## Acceptance

- L0 + L1 on 8822B HALMAC profile
- L2: H2C parm bytes + `rtw_macid_ctl_set_h2c_msr` side effects for single-macid connect/disconnect vectors

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)