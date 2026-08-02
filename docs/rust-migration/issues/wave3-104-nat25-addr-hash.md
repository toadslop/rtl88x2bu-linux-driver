---
title: "[W3-104] br_ext NAT25 addr/hash leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-104
epic: E05
blocked_by: [W3-103]
estimate_loc: 280
---

## Goal

Port helpers from [`core/rtw_br_ext.c`](../../../core/rtw_br_ext.c) to [`rust/rtw_br_ext.rs`](../../../rust/rtw_br_ext.rs):

- `__nat25_generate_ipv4_network_addr`
- `__nat25_generate_pppoe_network_addr`
- `__nat25_generate_ipv6_network_addr`
- `__nat25_network_hash`
- `__nat25_timeout`
- `__nat25_has_expired`
- `scan_tlv`
- `update_nd_link_layer_addr`
- `convert_ipv6_mac_to_mc`
- `skb_pull_and_merge`

## Notes

- Pure NAT25 address/hash/TLV leaf helpers; low HAL coupling.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for br_ext NAT25 addr/hash leaf helpers
