---
title: "[W3-119] odm adaptivity msg/parm leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-119
epic: E05
blocked_by: [W3-118]
estimate_loc: 210
---

## Goal

Port helpers from [`core/rtw_odm.c`](../../../core/rtw_odm.c) to [`rust/rtw_odm.rs`](../../../rust/rtw_odm.rs):

- `rtw_odm_adaptivity_ver_msg`
- `rtw_odm_adaptivity_en_msg`
- `rtw_odm_adaptivity_mode_msg`
- `rtw_odm_adaptivity_config_msg`
- `rtw_odm_adaptivity_needed`
- `rtw_odm_adaptivity_parm_msg`
- `rtw_odm_adaptivity_parm_set`
- `rtw_odm_get_perpkt_rssi`

## Notes

- Adaptivity debug/parm leaf helpers; adapter-coupled — expect bindgen + fixtures for L2.
- Spinlock shims (`rtw_odm_acquirespinlock` / `rtw_odm_releasespinlock`) stay in C for now.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for odm adaptivity msg/parm leaf helpers
