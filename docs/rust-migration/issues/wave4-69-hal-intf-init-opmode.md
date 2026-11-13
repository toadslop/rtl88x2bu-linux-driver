---
title: "[W4-69] Translate hal_intf.c — hal_init_opmode"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-69
epic: E06
blocked_by: [W4-68]
estimate_loc: 120
---

## Goal

Port `rtw_hal_init_opmode` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs).

## Notes

- Reads `mlme_priv` firmware state via existing shims; maps WIFI_* bits to `NDIS_802_11_NETWORK_INFRASTRUCTURE` values identically to C.
- Honor `CONFIG_AP_MODE` / `CONFIG_RTW_MESH` branches with the same `#[cfg]` gates as the C translation unit.
- Calls `rtw_setopmode_cmd` with `RTW_CMDF_DIRECTLY` through FFI — no change to command path semantics.

## Acceptance

- L0 + L1 symbol parity for `rtw_hal_init_opmode`
- Host L2 (optional): table-driven vectors for fw_state → networkType when harness lands; otherwise L0/L1 only

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-68
