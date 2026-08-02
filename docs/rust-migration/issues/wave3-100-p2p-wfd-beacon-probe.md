---
title: "[W3-100] p2p WFD IE builders (beacon/probe)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-100
epic: E05
blocked_by: [W3-99]
estimate_loc: 226
---

## Goal

Port helpers from [`core/rtw_p2p.c`](../../../core/rtw_p2p.c) to [`rust/rtw_p2p.rs`](../../../rust/rtw_p2p.rs):

- `build_beacon_wfd_ie`
- `build_probe_req_wfd_ie`

## Notes

- WFD IE builders for beacon/probe frames; complements W3-44 WFD helpers in `rtw_ieee80211.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for p2p WFD IE beacon/probe builders
