---
title: "[W3-82] sta keepalive and expire timeout"
labels: [rust-migration, phase-1, wave-3]
type: child
id: W3-82
epic: E05
blocked_by: [W3-81]
estimate_loc: 480
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `chk_sta_is_alive` (~48 LOC)
- `issue_aka_chk_frame` (~40 LOC)
- `rtw_check_restore_rf18` (~27 LOC)
- `expire_timeout_chk` (~369 LOC)

## Notes

- **Multi-PR slice (~480 LOC total)** — `plan-stacked-prs` must split into stacked PRs (≤250 changed lines each); `expire_timeout_chk` alone is ~369 LOC.
- Adapter/timer coupled.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sta keepalive and expire timeout
