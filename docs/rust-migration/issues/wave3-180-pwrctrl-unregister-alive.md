---
title: "[W3-180] Translate rtw_pwrctrl.c — unregister task/IO alive (part 2)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-180
epic: E05
blocked_by: [W3-179]
estimate_loc: 200
---

## Goal

Port the matching unregister helpers from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) into
[`rust/rtw_pwrctrl_alive.rs`](../../../rust/rtw_pwrctrl_alive.rs):

- `rtw_unregister_task_alive`
- `rtw_unregister_tx_alive`
- `rtw_unregister_cmd_alive`
- `rtw_unregister_rx_alive`
- `rtw_unregister_evt_alive`

## Notes

- Same module as [W3-179](wave3-179-pwrctrl-register-alive.md); completes the alive refcount lane.
- `rtw_btcoex_LpsVal` / `rtw_btcoex_IsBtControlLps` stay behind FFI when `CONFIG_BT_COEXIST` is enabled.

## Acceptance

- L0 + L1 on unregister symbols
- L2: host vectors for alives bitmask → expected RPWM downgrade when last task clears
