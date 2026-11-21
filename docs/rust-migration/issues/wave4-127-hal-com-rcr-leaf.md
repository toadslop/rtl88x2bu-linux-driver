---
title: "[W4-127] Translate hal_com_rest.c — RCR add/clear/check leaf"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-127
epic: E06
blocked_by: []
estimate_loc: 150
---

## Goal

Port RCR helper cluster from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
new `rust/hal_com_rcr.rs` (included from `hal_com.rs`):

- `rtw_hal_rcr_check`
- `rtw_hal_rcr_add`
- `rtw_hal_rcr_clear`

## Notes

- **Parallel lane:** disjoint Rust file from MBSSID/TSF — `rtw_hal_rcr_add`/`clear` use `rtw_hal_get_hwreg`/`set_hwreg` with `HW_VAR_RCR`, not the static `hw_var_rcr_*` helpers (defer those: `hw_var_rcr_config` on set, `hw_var_rcr_get` on get — the get path refreshes `ReceiveConfig` before add/clear compares cached `hal->ReceiveConfig`).
- Does not port `rtw_hal_rcr_set_chk_bssid` ([W4-128](wave4-128-hal-com-rcr-chk-bssid.md)).
- When porting `hw_var_rcr_config` later, `CONFIG_CUSTOMER_ALIBABA_GENERAL` full RCR mask replacement must match C `#ifdef`.

## Acceptance

- L0 + L1 for exported `rtw_hal_rcr_*` symbols
- L2: host stub traces `rtw_hal_set_hwreg`/`get_hwreg` RCR values on add/clear sequences

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)