---
title: "[W3-137] Translate rtw_btcoex_wifionly.c — wifionly notify stubs"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-137
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port the WiFi-only BTC coexistence stubs from [`core/rtw_btcoex_wifionly.c`](../../../core/rtw_btcoex_wifionly.c)
to [`rust/rtw_btcoex_wifionly.rs`](../../../rust/rtw_btcoex_wifionly.rs):

- `rtw_btcoex_wifionly_switchband_notify`
- `rtw_btcoex_wifionly_scan_notify`
- `rtw_btcoex_wifionly_connect_notify`
- `rtw_btcoex_wifionly_hw_config`
- `rtw_btcoex_wifionly_initialize`
- `rtw_btcoex_wifionly_AntInfoSetting`

## Notes

- Parallel lane vs W3-126/127 (`rtw_btcoex.c`); no `blocked_by` on those issues.
- Full HAL btcoex (`hal/hal_btcoex*.c`) is Wave 4 — this slice stays in `core/`.
- Functions are mostly no-ops or light adapter field updates — L0/L1 sufficient if no L2 path.

## Acceptance

- L0 + L1 symbol check on new Rust object linked into `88x2bu.ko`
