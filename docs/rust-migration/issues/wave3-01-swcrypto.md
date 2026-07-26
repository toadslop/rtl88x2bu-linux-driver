---
title: "[W3-01] Translate core/rtw_swcrypto.c"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-01
epic: E05
blocked_by: [W2-16, T2]
estimate_loc: 300
---

## Goal

Exact-translate [`core/rtw_swcrypto.c`](../../../core/rtw_swcrypto.c) (~296 LOC) to Rust; swap Makefile object. Thin wrappers around already-ported crypto (`ccmp_*`, `aes_siv_*`, `gcmp_*`, `bip_*`, `tdls_generate_tpk`).

## Notes

- Wave 2 tail: closes the crypto dependency chain before core protocol ports.
- Typed Rust internals may call existing `rust/ccmp.rs`, `rust/gcmp.rs`, etc.; preserve `extern "C"` names (`_rtw_ccmp_encrypt`, `_bip_ccmp_protect`, …) for C callers in `rtw_security.c`.
- Characterize frame/key edge cases (128 vs 256-bit CCMP/GCMP, PV1 hdrlen 26) with L2 vectors extending the host harness.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness where applicable)
- L3 only if init touched (not expected)
