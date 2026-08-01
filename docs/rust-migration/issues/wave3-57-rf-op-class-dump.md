---
title: "[W3-57] Translate rtw_rf.c — op-class debug dump helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-57
epic: E05
blocked_by: [W3-56]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `dbg_global_op_class_validate`
- `dump_global_op_class`
- `dump_opc_pref_single`
- `dump_cap_spt_op_class_ch`
- `dump_reg_spt_op_class_ch`
- `dump_cur_spt_op_class_ch`

## Notes

- Debug/proc-only helpers; no HAL MMIO. Low coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for op-class dump formatters (proc/debug path)
