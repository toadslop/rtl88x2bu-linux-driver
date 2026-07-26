---
title: "[W2-17] Translate rtw_chplan lookup helpers"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-17
epic: E04
blocked_by: [W2-16, T2]
estimate_loc: 200
---

## Goal

Port chplan lookup / DFS / country helpers from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c) to [`rust/rtw_chplan.rs`](../../../rust/rtw_chplan.rs). Static tables remain in C; Rust reads them via `extern`.

## Acceptance

- L0 + L1 + L2 (`tests/host/chplan/`) green
- Tables stay in C until a dedicated data migration PR
