---
title: "[W4-144] Translate hal_com_rest.c — BT coexist WLAN port H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-144
epic: E06
blocked_by: []
estimate_loc: 40
---

## Goal

Port `rtw_hal_set_wifi_btc_port_id_cmd` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_btc_port.rs` when `CONFIG_BT_COEXIST` and
`CONFIG_FW_MULTI_PORT_SUPPORT` are enabled.

## Notes

- **Parallel lane:** single H2C packer — uses `rtw_hal_get_port` (W4-48).
- Pair with host L2 stub in the same issue stack if LOC is under ~100 after harness.

## Acceptance

- L0 + L1 on multi-port + BT coexist profile
- L2: `SET_H2CCMD_BTC_WL_PORT_ID` byte vs seeded `hw_port`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)