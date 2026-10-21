---
title: "[W4-52] Translate hal_com_rest.c — security CAM register read/write"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-52
epic: E06
blocked_by: [W4-51]
estimate_loc: 120
---

## Goal

Port security CAM MMIO accessors from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_sec_read_cam` — `REG_CAMCMD` poll + `REG_CAMREAD`
- `rtw_sec_write_cam` — `REG_CAMWRITE` + write poll

## Notes

- Preserve `sec_cam_access_mutex`, surprise-remove early exit, and `SEC_CAM_ACCESS_TIMEOUT_MS` polling loops.
- Chains after [W4-51](wave4-51-hal-com-reqtxrpt-mi-mac.md).

## Acceptance

- L0 on pinned profile
- L1 symbols for both functions
- Debug logging under `DBG_SEC_CAM_ACCESS` / timeout paths unchanged
