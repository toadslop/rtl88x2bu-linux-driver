---
title: "[W3-74] Translate rtw_ap.c — STA security IE parse and policy"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-74
epic: E05
blocked_by: [W3-73]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ap.c`](../../../core/rtw_ap.c) to [`rust/rtw_ap_rest.rs`](../../../rust/rtw_ap_rest.rs):

- `rtw_ap_parse_sta_security_ie`

## Notes

- RSN/WPA parse + policy gates on assoc path.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for STA security IE parse
