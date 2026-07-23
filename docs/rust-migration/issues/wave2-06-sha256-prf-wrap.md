---
title: "[W2-06] Translate sha256-prf.c + rtw_crypto_wrap.c"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-06
epic: E04
blocked_by: [W2-05]
estimate_loc: 200
---

## Goal

Exact-translate [`core/crypto/sha256-prf.c`](../../../core/crypto/sha256-prf.c) (~109) and [`core/crypto/rtw_crypto_wrap.c`](../../../core/crypto/rtw_crypto_wrap.c) (~85) together if the combined PR stays near ~200 LOC; otherwise split.

## Acceptance

- Both objects swapped to Rust; smoke passes
