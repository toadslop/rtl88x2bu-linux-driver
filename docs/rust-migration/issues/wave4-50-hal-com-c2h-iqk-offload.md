---
title: "[W4-50] Translate hal_com_rest.c — C2H IQK / txpwr offload wait"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-50
epic: E06
blocked_by: [W4-49]
estimate_loc: 80
---

## Goal

Port firmware offload completion helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `c2h_iqk_offload` + `c2h_iqk_offload_wait` (`CONFIG_FW_C2H_PKT`)
- `c2h_txpwr_idx_offload_done` + `c2h_txpwr_idx_offload_wait` (`CONFIG_FW_OFFLOAD_SET_TXPWR_IDX`)

## Notes

- Thin `rtw_sctx_done` / `rtw_sctx_wait` wrappers around `HAL_DATA_TYPE` submit contexts — preserve timeout and submit_time logging.
- Opens hal_com_rest tranche 10 after [W4-49](wave4-49-hal-com-hw-port-ctrl.md).

## Acceptance

- L0 with FW C2H / txpwr offload Kconfig enabled on pinned profile
- L1 symbols for all four functions
- `extern "C"` signatures unchanged for C2H dispatch tables
