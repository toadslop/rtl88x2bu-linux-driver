---
title: "[W3-06] Translate rtw_security.c part 1 — type string helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-06
epic: E05
blocked_by: [W3-01]
estimate_loc: 80
---

## Goal

Port pure string-mapping helpers from [`core/rtw_security.c`](../../../core/rtw_security.c):

- `security_type_str`
- `security_type_bip_to_gmcs` (`CONFIG_IEEE80211W`)

Extract to `rtw_security_rest.c`; begin multi-part port of the 2872-line file.

## Notes

- Introduce `SecurityType` enum at Rust API (A2 or inline); `extern "C"` shims return C string pointers matching C lifetime semantics.
- L2: exhaustive enum value → string / cipher mapping table.

## Acceptance

- L0 build + L1 symbols + L2 host vectors
