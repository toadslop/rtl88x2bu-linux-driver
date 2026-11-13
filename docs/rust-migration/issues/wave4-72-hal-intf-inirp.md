---
title: "[W4-72] Translate hal_intf.c — inirp init/deinit"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-72
epic: E06
blocked_by: [W4-71]
estimate_loc: 80
---

## Goal

Port USB/PCI IN-request pipe wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_inirp_init`
- `rtw_hal_inirp_deinit`

## Notes

- Behind `CONFIG_USB_HCI || CONFIG_PCI_HCI`; secondary adapters return `_SUCCESS` without calling `hal_func`.
- `is_primary_adapter` check order and return values must match C.

## Acceptance

- L0 + L1 symbol parity for both functions
- L3 smoke on USB HCI build in QEMU when feasible

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-71
