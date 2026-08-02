---
title: "[W3-98] p2p channel/negotiation leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-98
epic: E05
blocked_by: [W3-97]
estimate_loc: 195
---

## Goal

Port helpers from [`core/rtw_p2p.c`](../../../core/rtw_p2p.c) to [`rust/rtw_p2p.rs`](../../../rust/rtw_p2p.rs):

- `rtw_p2p_is_channel_list_ok`
- `rtw_p2p_get_peer_ch_list`
- `rtw_p2p_ch_inclusion`
- `rtw_p2p_nego_intent_compare`
- `process_p2p_cross_connect_ie`
- `process_p2p_ps_ie`

## Notes

- Low-coupling P2P channel/negotiation leaf helpers; starts tranche 6 on `rtw_p2p.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for p2p channel/negotiation leaf helpers
