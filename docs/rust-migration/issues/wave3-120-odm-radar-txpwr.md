---
title: "[W3-120] odm radar detect and tx power leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-120
epic: E05
blocked_by: [W3-119]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_odm.c`](../../../core/rtw_odm.c) to [`rust/rtw_odm.rs`](../../../rust/rtw_odm.rs):

- `rtw_odm_get_tx_power_mbm`
- `rtw_odm_radar_detect_reset`
- `rtw_odm_radar_detect_disable`
- `rtw_odm_radar_detect_enable`
- `rtw_odm_radar_detect`
- `rtw_odm_update_dfs_region`
- `rtw_odm_radar_detect_polling_int_ms`

## Notes

- Radar-detect and tx-power leaf helpers; completes initial `rtw_odm.c` export surface.
- Debug dump helpers (`debug_*`) and chinfo parse stay in C until a later tranche.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for odm radar detect and tx power leaf helpers
