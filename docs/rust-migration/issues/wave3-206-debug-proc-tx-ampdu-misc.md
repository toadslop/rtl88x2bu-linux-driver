---
title: "[W3-206] Translate rtw_debug_rest.c — proc tx AMPDU misc leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-206
epic: E05
blocked_by: [W3-204]
estimate_loc: 160
---

## Goal

Port remaining TX-side AMPDU-related debug procs from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_ampdu.rs`](../../../rust/rtw_debug_proc_ampdu.rs):

- `proc_get_tx_quick_addba_req` / `proc_set_tx_quick_addba_req`
- `proc_get_tx_aval_th` / `proc_set_tx_aval_th` (`CONFIG_SDIO_TX_ENABLE_AVAL_INT`)

## Notes

- Chains on [W3-204](wave3-204-debug-proc-rx-ampdu.md) (shared module).
- `tx_aval_*` touches `dvobj_priv` only — no MMIO.

## Acceptance

- L0 with relevant Kconfig symbols enabled on pinned profile
- L1 on all handlers
- L2: registry/dvobj fixture — proc_set then proc_get golden strings
