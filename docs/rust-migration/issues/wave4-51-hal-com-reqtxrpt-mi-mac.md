---
title: "[W4-51] Translate hal_com_rest.c — reqtxrpt + MI MAC addr push"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-51
epic: E06
blocked_by: [W4-50]
estimate_loc: 60
---

## Goal

Port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_hal_reqtxrpt` — delegate to `hal_func.reqtxrpt` when present
- `rtw_mi_set_mac_addr` — `CONFIG_MI_WITH_MBSSID_CAM` vs per-iface `HW_VAR_MAC_ADDR` loop

## Notes

- Chains hal_com lane after [W4-50](wave4-50-hal-com-c2h-iqk-offload.md).

## Acceptance

- L0 on pinned profile
- L1 symbols for both functions
- MI path preserves iface iteration and `rtw_hal_set_hwreg` calls
