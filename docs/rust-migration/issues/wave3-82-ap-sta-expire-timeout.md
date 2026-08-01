---
title: "[W3-82] sta keepalive and expire timeout"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-82
epic: E05
blocked_by: [W3-81]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rust/rtw_ap_rest.rs`](../../../rust/rust/rtw_ap_rest.rs):

- `chk_sta_is_alive`
- `issue_aka_chk_frame`
- `rtw_check_restore_rf18`
- `expire_timeout_chk`

## Notes

- Adapter/timer coupled; may split into two PRs at implement time if diff >250 lines.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sta keepalive and expire timeout
