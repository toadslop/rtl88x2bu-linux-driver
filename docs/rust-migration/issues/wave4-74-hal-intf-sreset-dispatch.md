---
title: "[W4-74] Translate hal_intf.c — silent reset dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-74
epic: E06
blocked_by: [W4-73]
estimate_loc: 90
---

## Goal

Port `DBG_CONFIG_ERROR_DETECT` silent-reset wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_sreset_init`
- `rtw_hal_sreset_reset` (primary-adapter promotion)
- `rtw_hal_sreset_reset_value`
- `rtw_hal_sreset_xmit_status_check`
- `rtw_hal_sreset_linked_status_check`
- `rtw_hal_sreset_get_wifi_status`
- `rtw_hal_sreset_inprogress`

## Notes

- Preserve `GET_PRIMARY_ADAPTER` behavior on reset/inprogress entry points.
- Gated by `DBG_CONFIG_ERROR_DETECT` like C — USB profile builds unchanged when disabled.

## Acceptance

- L0 with `DBG_CONFIG_ERROR_DETECT` enabled on pinned profile
- L1 symbol parity for all seven symbols

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-73