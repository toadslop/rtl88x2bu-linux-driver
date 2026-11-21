---
title: "[W4-128] Translate hal_com_rest.c — RCR check-BSSID policy"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-128
epic: E06
blocked_by: [W4-127]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_com_rcr.rs`](../../../rust/hal_com_rcr.rs) with
`rtw_hal_rcr_set_chk_bssid` and `rtw_hal_rcr_set_chk_bssid_act_non` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c).

## Notes

- **RCR lane part 2** — uses `rtw_mi_status_*` / `rtw_mi_status_merge` via existing FFI; preserve `CONFIG_MI_WITH_MBSSID_CAM`, `CONFIG_TDLS`, `CONFIG_AP_MODE`, `CONFIG_RTW_MESH`, `CONFIG_RTW_MULTI_AP` cfg branches.
- **Same slice:** port static `rtw_hal_tsf_update_pause` and `rtw_hal_tsf_update_restore` into `rust/hal_com_tsf.rs` (or co-located `hal_com_rcr.rs` helpers) — they are file-static in C today and are called from this function when TSF filter bits change; do not leave them as unresolved C-only symbols.
- Early return when computed `rcr_new == rcr` before `hw_var_set_hwreg`.

## Acceptance

- L0/L1 with pinned concurrent profile
- L2: table-driven `self_action` × `mi_state` → expected RCR bit masks (subset of actions: scan enter/done, STA connect/disconnect, `MLME_ACTION_NONE`)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-127