---
title: "[W3-69] Translate rtw_mlme_ext.c — delba timeout and peer alive checks"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-69
epic: E05
blocked_by: [W3-68]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `rtw_delba_check`
- `chk_ap_is_alive`
- `chk_adhoc_peer_is_alive`
- `chk_tdls_peer_sta_is_alive`

## Notes

- Peer liveness predicates; delba may call issue_del_ba* via C shim.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for delba/peer-alive checks
