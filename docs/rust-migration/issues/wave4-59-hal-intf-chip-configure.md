---
title: "[W4-59] Translate hal_intf.c — chip configure + read_chip_info"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-59
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port early HAL bring-up entry points from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c)
into [`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_chip_configure`
- `rtw_hal_read_chip_info` (including HCI-type dispatch and `rtw_read_chip_id` path)

## Notes

- Extends the existing `hal_intf` Rust module (W4-07/08/36); extract only these bodies
  before enabling `CONFIG_RUST` object swap for the symbols.
- Preserve `_SUCCESS` / `_FAIL` returns and every `RTW_ERR` / `RTW_INFO` line from C.

## Acceptance

- L0: `make KDIR=/opt/linux LLVM=1` links `rust/hal_intf.o` + `hal_intf_rest.o`
- L1: symbol parity for both functions
- L3: QEMU insmod/rmmod unchanged (no new Oops on default 8822BU build)
