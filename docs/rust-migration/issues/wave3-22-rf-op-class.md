---
title: "[W3-22] Translate rtw_rf.c — global op-class lookup"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-22
epic: E05
blocked_by: [W3-21]
estimate_loc: 200
---

## Goal

Port global operating-class lookup helpers from [`core/rtw_rf.c`](../../../core/rtw_rf.c) to [`rust/rtw_rf_rest.rs`](../../../rust/rtw_rf_rest.rs):

- `is_valid_global_op_class_id`, `get_sub_op_class`
- `rtw_get_op_class_by_chbw`, `rtw_get_bw_offset_by_op_class_ch`

## Notes

- `global_op_class[]` table and `OP_CLASS_ENT` macros move with the helpers.
- `dump_global_op_class` stays in C (debug output) until a debug-dump issue.
- `op_class_pref_*` adapter state stays in C.
- L2: host harness with ch/bw/offset → op-class oracle vectors.

## Acceptance

- L0 build + L2 host unit tests for op-class lookup
