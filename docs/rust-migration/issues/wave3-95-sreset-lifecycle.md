---
title: "[W3-95] silent reset lifecycle"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-95
epic: E05
blocked_by: [W3-94]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_sreset.c`](../../../core/rtw_sreset.c) to [`rust/rtw_sreset.rs`](../../../rust/rtw_sreset.rs):

- `sreset_init_value`
- `sreset_reset_value`
- `sreset_get_wifi_status`
- `sreset_set_wifi_error_status`
- `sreset_inprogress`
- `sreset_restore_security_station`
- `sreset_stop_adapter`
- `sreset_start_adapter`
- `sreset_reset`

## Notes

- HAL/adapter coupled recovery path; new Rust module.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for silent reset lifecycle
