---
title: "[W3-101] p2p WFD IE builders (probe-resp/assoc)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-101
epic: E05
blocked_by: [W3-100]
estimate_loc: 296
---

## Goal

Port helpers from [`core/rtw_p2p.c`](../../../core/rtw_p2p.c) to [`rust/rtw_p2p.rs`](../../../rust/rtw_p2p.rs):

- `build_probe_resp_wfd_ie`
- `build_assoc_req_wfd_ie`
- `build_assoc_resp_wfd_ie`

## Notes

- WFD IE builders for probe-resp/assoc frames; completes WFD builder cluster in `rtw_p2p.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for p2p WFD IE probe-resp/assoc builders
