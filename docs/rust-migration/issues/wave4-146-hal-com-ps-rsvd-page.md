---
title: "[W4-146] Translate hal_com_rest.c — PS reserved-page join report"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-146
epic: E06
blocked_by: []
estimate_loc: 25
---

## Goal

Under `CONFIG_FW_MULTI_PORT_SUPPORT`, port `rtw_set_ps_rsvd_page` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_ps_rsvd.rs`.

## Notes

- **Parallel lane:** LPS/FW power-save helper — calls `rtw_hal_set_hwreg` with
  `HW_VAR_H2C_FW_JOINBSSRPT` and `RT_MEDIA_CONNECT` when iface differs from
  `pwrctl->fw_psmode_iface_id`.
- Early return when `adapter->iface_id == pwrctl->fw_psmode_iface_id`.

## Acceptance

- L0 + L1 with multi-port profile enabled
- L2: stub records whether join report HWVAR fires for matching vs mismatched iface id

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)