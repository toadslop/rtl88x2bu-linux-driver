---
title: "[W3-01] Translate rtw_swcrypto CCMP/GCMP wrappers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-01
epic: E05
blocked_by: [W2-17]
estimate_loc: 190
---

## Goal

Port `_rtw_ccmp_*` and `_rtw_gcmp_*` from [`core/rtw_swcrypto.c`](../../../core/rtw_swcrypto.c) to [`rust/rtw_swcrypto.rs`](../../../rust/rtw_swcrypto.rs).

## Acceptance

- L0 build with CONFIG_RUST; no duplicate symbols with C
- `_tdls_generate_tpk` remains in C (sta_info layout)
