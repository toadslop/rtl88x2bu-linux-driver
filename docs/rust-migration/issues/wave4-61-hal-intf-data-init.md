---
title: "[W4-61] Translate hal_intf.c — hal_data_init (part 1)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-61
epic: E06
blocked_by: [W4-60]
estimate_loc: 200
---

## Goal

Port the first ~200 LOC tranche of `rtw_hal_data_init` from
[`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into [`rust/hal_intf.rs`](../../../rust/hal_intf.rs)
— allocator setup, `HalData` memset, and early capability flags before DM/HCI sub-inits.

## Notes

- Split `rtw_hal_data_init` across W4-61 / W4-62 if the full function exceeds one PR
  (Makefile extract to `hal_intf_rest.c` per multi-part port rules).
- Part 2 (if needed) continues in a follow-up issue chained on W4-61.

## Acceptance

- L0 + L1 for every symbol moved in this tranche
- L3: insmod still registers `rtl88x2bu` without Oops/WARN on default config
