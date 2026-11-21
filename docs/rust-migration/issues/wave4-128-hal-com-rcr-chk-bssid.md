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

- **RCR lane part 2** — uses `rtw_mi_status_*` / `rtw_mi_status_merge` via existing FFI; preserve `CONFIG_TDLS`, `CONFIG_AP_MODE`, `CONFIG_RTW_MESH`, `CONFIG_RTW_MULTI_AP`, and `CONFIG_CLIENT_PORT_CFG` branches.
- Pinned concurrent 8822B sets both `CONFIG_MI_WITH_MBSSID_CAM` and `CONFIG_CLIENT_PORT_CFG`, so the `#if defined(CONFIG_MI_WITH_MBSSID_CAM) && !defined(CONFIG_CLIENT_PORT_CFG)` shortcut is **not** taken — implement the full `mi_state` decision tree.
- Port static `rtw_hal_tsf_update_pause`/`restore` and call only when `!hal_spec->rx_tsf_filter`: **pause** before `HW_VAR_RCR` write when old `rcr` had `RCR_CBSSID_BCN` and `rcr_new` clears it; **restore** after the write when old lacked `RCR_CBSSID_BCN` and `rcr_new` sets it **unless** `self_action == MLME_STA_CONNECTING`. Under `CONFIG_MI_WITH_MBSSID_CAM` the helper bodies are empty (no-op on pinned profile).
- Early return when computed `rcr_new == rcr` before `rtw_hal_set_hwreg(adapter, HW_VAR_RCR, …)`.

## Acceptance

- L0/L1 with pinned concurrent profile
- L2: table-driven `self_action` × `mi_state` → expected RCR bit masks (subset of actions: scan enter/done, STA connect/disconnect, `MLME_ACTION_NONE`)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-127