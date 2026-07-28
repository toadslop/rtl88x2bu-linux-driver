---
title: "[W3-34] Translate rtw_rm_util.c — RM token generation"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-34
epic: E05
blocked_by: [W3-33]
estimate_loc: 200
---

## Goal

Port RM token/ID generation helpers from [`core/rtw_rm_util.c`](../../../core/rtw_rm_util.c) to [`rust/rtw_rm_util.rs`](../../../rust/rtw_rm_util.rs):

- `rm_gen_dialog_token`, `rm_gen_meas_token`, `rm_gen_rmid`

## Notes

- Uses adapter state for uniqueness; may need thin C shim for `padapter` access.
- L2: host harness with deterministic token sequences.

## Acceptance

- L0 build + L2 host unit tests for RM token generation
