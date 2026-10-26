---
title: "[W3-179] Translate rtw_pwrctrl.c — register task/IO alive (part 1)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-179
epic: E05
blocked_by: [W3-175]
estimate_loc: 220
---

## Goal

Port the “raise CPWM before I/O” register helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_alive.rs`](../../../rust/rtw_pwrctrl_alive.rs):

- `rtw_register_task_alive`
- `rtw_register_tx_alive`
- `rtw_register_cmd_alive`
- `rtw_register_rx_alive`
- `rtw_register_evt_alive`

## Notes

- **Dependency rationale:** may call `rtw_set_rpwm` — land [W3-175](wave3-175-pwrctrl-rpwm-cpwm.md) first (or stack on its open PR branch).
- `register_task_alive` / `unregister_task_alive` bit helpers may stay in C or move in the follow-up slice [W3-180](wave3-180-pwrctrl-unregister-alive.md).
- `CONFIG_DETECT_CPWM_BY_POLLING` and `CONFIG_BT_COEXIST` branches must match C.

## Acceptance

- L0 + L1 on swapped register symbols
- L2: mocked `pwrctrl_priv` with CPWM/RPWM fields — success/fail paths for S2 raise
