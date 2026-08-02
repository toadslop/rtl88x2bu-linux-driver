---
title: "[W3-99] p2p P2P IE frame builders"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-99
epic: E05
blocked_by: [W3-98]
estimate_loc: 268
---

## Goal

Port helpers from [`core/rtw_p2p.c`](../../../core/rtw_p2p.c) to [`rust/rtw_p2p.rs`](../../../rust/rtw_p2p.rs):

- `build_beacon_p2p_ie`
- `build_probe_resp_p2p_ie`
- `build_prov_disc_request_p2p_ie`
- `build_assoc_resp_p2p_ie`
- `build_deauth_p2p_ie`

## Notes

- Frame-context P2P IE builders; complements W3-43 generic P2P IE merge/delete in `rtw_ieee80211.c`.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for p2p P2P IE frame builders
