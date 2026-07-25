---
title: "[W3-01] Translate rtw_swcrypto CCMP/GCMP wrappers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-01
epic: E05
blocked_by: [W2-16, T2]
estimate_loc: 190
---

## Goal

Port `_rtw_ccmp_*` and `_rtw_gcmp_*` from [`core/rtw_swcrypto.c`](../../../core/rtw_swcrypto.c) to [`rust/rtw_swcrypto.rs`](../../../rust/rtw_swcrypto.rs). Thin wrappers around already-ported crypto (`ccmp_*`, `gcmp_*`).

## Notes

- Wave 2 tail: first slice of `rtw_swcrypto.c` before core protocol ports.
- Typed Rust internals call existing `rust/ccmp.rs`, `rust/gcmp.rs`; preserve `extern "C"` names for C callers in `rtw_security.c`.
- Characterize frame/key edge cases (128 vs 256-bit CCMP/GCMP, PV1 hdrlen 26) with L2 vectors extending the host harness.
- In-flight: `cursor/w3-01-swcrypto-ccmp-gcmp-3dd4`.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 harness where applicable)
- `_tdls_generate_tpk` remains in C; W3-02 extracts it to `rtw_swcrypto_rest.c` before dropping `rtw_swcrypto.o` (port deferred — `sta_info` layout)
