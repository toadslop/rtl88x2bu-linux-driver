---
title: "[W3-109] wapi PN/IE/CAM table leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-109
epic: E05
blocked_by: [W3-108]
estimate_loc: 220
---

## Goal

Port helpers from [`core/rtw_wapi.c`](../../../core/rtw_wapi.c) to [`rust/rtw_wapi.rs`](../../../rust/rtw_wapi.rs):

- `WapiComparePN`
- `WapiSetIE`
- `WapiGetEntryForCamWrite`
- `WapiGetEntryForCamClear`
- `WapiResetAllCamEntry`

## Notes

- WAPI PN compare and CAM table leaf helpers; low-medium coupling.
- **Kbuild / `CONFIG_*`:** `core/rtw_wapi.o` is linked only when **`CONFIG_WAPI_SUPPORT=y`**
  (disabled in the default 88x2bu profile).
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for wapi PN/IE/CAM table leaf helpers
