---
title: "[W3-76] element parse (parse_elems)"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-76
epic: E05
blocked_by: [W3-75]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `rtw_ieee802_11_parse_elems`
- `rtw_ieee802_11_parse_vendor_specific`

## Notes

- Pure IE byte parser; complements W3-03 get/remove helpers. L2: host harness with malformed/valid element vectors.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for element parse helpers
