---
title: "[W4-147] Translate hal_com_rest.c — P2P PS HALMAC offload"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-147
epic: E06
blocked_by: []
estimate_loc: 130
---

## Goal

Under `CONFIG_P2P_PS` and `RTW_HALMAC`, port `rtw_set_p2p_ps_offload_cmd` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_p2p_ps.rs`.

## Notes

- **Parallel lane:** fills `HAL_P2P_PS_PARA`, invokes `rtw_halmac_p2pps`, mirrors
  result into `hal->p2p_ps_offload`.
- Preserve `P2P_PS_SCAN` / `P2P_PS_SCAN_DONE` early returns (FW not ready).
- CTWindow, NoA loop, and `CONFIG_P2P_PS_NOA_USE_MACID_SLEEP` branches must match C.

## Acceptance

- L0 + L1 on stock 8822B profile (`CONFIG_P2P_PS` enabled)
- L2: golden `HAL_P2P_PS_PARA` for disable, enable with NoA, and STA-missing error path

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)