---
title: "[W3-126] btcoex init and notify leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-126
epic: E05
blocked_by: [W3-125]
estimate_loc: 210
---

## Goal

Port helpers from [`core/rtw_btcoex.c`](../../../core/rtw_btcoex.c) to [`rust/rtw_btcoex.rs`](../../../rust/rtw_btcoex.rs):

- `rtw_btcoex_Initialize`
- `rtw_btcoex_PowerOnSetting`
- `rtw_btcoex_AntInfoSetting`
- `rtw_btcoex_PowerOffSetting`
- `rtw_btcoex_PreLoadFirmware`
- `rtw_btcoex_HAL_Initialize`
- `rtw_btcoex_IpsNotify`
- `rtw_btcoex_LpsNotify`
- `rtw_btcoex_ScanNotify`
- `rtw_btcoex_MediaStatusNotify`

## Notes

- BT coexistence init/notify leaf helpers; starts tranche 7 on `rtw_btcoex.c`.
- Handler/AMPDU/policy helpers ship in W3-127.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for btcoex init and notify leaf helpers
