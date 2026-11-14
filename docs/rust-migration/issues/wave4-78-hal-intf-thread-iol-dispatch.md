---
title: "[W4-78] Translate hal_intf.c — IOL + thread-mode dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-78
epic: E06
blocked_by: [W4-76]
estimate_loc: 70
---

## Goal

Port optional thread/IOL dispatch wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_iol_cmd` (`CONFIG_IOL`)
- `rtw_hal_xmit_thread_handler` (`CONFIG_XMIT_THREAD_MODE`)
- `rtw_hal_recv_hdl` (`CONFIG_RECV_THREAD_MODE`)

## Notes

- Thin `hal_func` pointers only; IOL sync exec stays in chip ops.
- Chains hal_intf lane after [W4-76](wave4-76-hal-intf-fw-mem-clear-int.md).

## Acceptance

- L0 on profile with relevant `CONFIG_*` flags enabled
- L1 symbol parity for each compiled symbol

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-76