---
title: "[W4-68] Translate hal_intf.c — sw_led init/deinit"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-68
epic: E06
blocked_by: [W4-67]
estimate_loc: 80
---

## Goal

Port software LED hooks from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_sw_led_init`
- `rtw_hal_sw_led_deinit`

## Notes

- Entire slice behind `CONFIG_RTW_SW_LED`; mirror `#[cfg]` from C so non-LED builds link the same symbols.
- Early returns when `bRegUseLed` is false or adapter is not primary — match C guard order.
- Dispatch through `hal_func.InitSwLeds` / `DeInitSwLeds` and existing `rtw_led_*` helpers via FFI shims.

## Acceptance

- L0 + L1 symbol parity for both functions (LED cfg on and off matrix as applicable)
- L3 smoke: no new WARN on module load/unload in QEMU

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** #1985 (`W4-67`)
