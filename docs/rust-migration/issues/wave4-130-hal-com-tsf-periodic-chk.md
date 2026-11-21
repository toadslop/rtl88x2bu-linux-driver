---
title: "[W4-130] Translate hal_com_rest.c — TSF periodic update check"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-130
epic: E06
blocked_by: [W4-128, W4-129]
estimate_loc: 180
---

## Goal

Extend `rust/hal_com_tsf.rs` with periodic TSF restore logic from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c):

- `rtw_hal_periodic_tsf_update_chk`
- `rtw_hal_periodic_tsf_update_end_timer_hdl`

## Notes

- Calls `rtw_hal_rcr_set_chk_bssid(adapter, MLME_ACTION_NONE)` — needs W4-128 RCR policy on stack or `master`.
- Uses `dvobj->periodic_tsf_update_etime` and `_set_timer` for restore window; wire FFI for timer/set_timer as in other HAL ports.
- When `periodic_tsf_update_etime != 0`, run the **expiry branch** (clear `etime`, call `rtw_hal_rcr_set_chk_bssid`, return) — not a silent skip.
- Skip main restore loop only when `offch_state != OFFCHS_NONE`.
- Entire function body is empty under `CONFIG_MI_WITH_MBSSID_CAM` (default concurrent 8822B profile) — L2 may noop-gate like C.

## Acceptance

- L0 + L1
- L2: synthetic `mi_state` + pause timestamps → expected `restore_ms`, timer arm, and single RCR chk_bssid invocation

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-128, W4-129