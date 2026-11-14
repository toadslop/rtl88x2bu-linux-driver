---
title: "[W4-82] Translate hal_intf.c — BB/RF register dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-82
epic: E06
blocked_by: [W4-81]
estimate_loc: 120
---

## Goal

Port BB/RF register accessors from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_read_bbreg` / `rtw_hal_write_bbreg`
- `rtw_hal_read_rfreg` / `rtw_hal_write_rfreg` (preserve `DBG_IO` sniff logging + `PHY_CalculateBitShift` formatting)
- Optional `CONFIG_SYSON_INDIRECT_ACCESS` `read_syson_reg` / `write_syson_reg`
- `CONFIG_PCI_HCI` `rtw_udelay_os(2)` after RF write on non-Jaguar paths

## Notes

- Thin `hal_func` dispatch — no MMIO in Rust beyond existing chip ops.
- Chains hal_intf lane after [W4-81](wave4-81-hal-intf-hal-init.md).

## Acceptance

- L0 build with `DBG_IO` on/off
- L1 symbols exported from `rust/hal_intf.o`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-81