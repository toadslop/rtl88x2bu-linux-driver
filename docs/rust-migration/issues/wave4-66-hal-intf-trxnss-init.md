---
title: "[W4-66] Translate hal_intf.c — hal_trxnss_init"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-66
epic: E06
blocked_by: [W4-65]
estimate_loc: 120
---

## Goal

Port `rtw_hal_trxnss_init` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs).

## Notes

- Depends on HAL RF path fields populated by `rtw_hal_rfpath_init` (W4-65); reuse W4-08 `_dump_trx_nss` for debug output.
- Preserve `CONFIG_RTW_TX_NPATH_EN` hardware-type branches for `txpath_cap_num_nss[]`.
- Clamp TX/RX NSS using registry limits and `rf_type_to_rf_rx_cnt` via C shims.

## Acceptance

- L0 + L1 for `rtw_hal_trxnss_init`
- L2: host vectors for NSS clamp + `txpath_cap_num_nss` table for 8822BU-relevant paths
