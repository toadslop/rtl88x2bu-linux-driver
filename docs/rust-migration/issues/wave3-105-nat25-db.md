---
title: "[W3-105] br_ext NAT25 DB lifecycle"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-105
epic: E05
blocked_by: [W3-104]
estimate_loc: 165
---

## Goal

Port helpers from [`core/rtw_br_ext.c`](../../../core/rtw_br_ext.c) to [`rust/rtw_br_ext.rs`](../../../rust/rtw_br_ext.rs):

- `__nat25_db_network_lookup_and_replace`
- `__nat25_db_network_insert`
- `nat25_db_expire`
- `nat25_db_cleanup`

## Notes

- NAT25 DB insert/lookup/expire; medium coupling via adapter NAT25 table.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for br_ext NAT25 DB lifecycle helpers
