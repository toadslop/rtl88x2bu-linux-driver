---
title: "[W3-04] Translate rtw_security.c part 1 — type string helpers"
labels: [rust-migration, phase-1, wave-3, size/~100]
type: child
id: W3-04
epic: E05
blocked_by: [A3, W3-02, T5]
estimate_loc: 80
---

## Goal

Port pure string-mapping helpers from [`core/rtw_security.c`](../../../core/rtw_security.c):

- `security_type_str`
- `security_type_bip_to_gmcs` (`CONFIG_IEEE80211W`)

Extract to `rtw_security_rest.c`; begin multi-part port of the 2872-line file.

## Notes

- Uses `SecurityType` enum from **A3** (`arch-03-domain-types-security.md`); `extern "C"` shims return C string pointers matching C lifetime semantics.
- L2: exhaustive enum value → string / cipher mapping table (T5 harness).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T5)
