---
title: "[W3-47] Translate rtw_recv.c — PN replay decache"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-47
epic: E05
blocked_by: [W3-46]
estimate_loc: 200
---

## Goal

Port PN replay decache helpers from [`core/rtw_recv.c`](../../../core/rtw_recv.c) to [`rust/rtw_recv.rs`](../../../rust/rtw_recv.rs):

- `recv_ucast_pn_decache`, `recv_bcast_pn_decache`, `recv_decache`

## Notes

- Security priv state coupling; needs populated sta/security fixtures for L2.
- Reorder/amsdu/HAL recv thread paths stay in C.
- L2: extend `tests/host/recv/` with PN/tid replay vectors.

## Acceptance

- L0 build + L2 host unit tests for PN replay decache helpers
