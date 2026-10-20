---
title: "[W4-48] Translate hal_com_rest.c — hw port query + HalData defaults"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-48
epic: E06
blocked_by: []
estimate_loc: 140
---

## Goal

Port leaf helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_hal_get_port` — `get_hw_port` / `CONFIG_CLIENT_PORT_CFG` / `DBG_HW_PORT` warn paths
- `rtw_init_hal_com_default_value` — AntDetection, offload flags, `RTW_REDUCE_SCAN_SWITCH_CH_TIME` ch_switch tweak

## Notes

- Parallel hal_com lane (disjoint from [W4-47](wave4-47-hal-com-c2h-evt-read.md)).
- `rtw_hal_reqtxrpt` stays in C (single hal_func indirection).

## Acceptance

- L0 + L1 exported symbols unchanged at ABI boundary
- L2 optional: host fixture for `registry_priv` → `HalData` default fields
