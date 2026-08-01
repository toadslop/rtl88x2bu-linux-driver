---
title: "[W3-53] Translate rtw_mlme.c — BSSID getters and same-network compare"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-53
epic: E05
blocked_by: [W3-52]
estimate_loc: 200
---

## Goal

Port MLME BSSID/compare helpers from [`core/rtw_mlme.c`](../../../core/rtw_mlme.c) to [`rust/rtw_mlme_rest.rs`](../../../rust/rtw_mlme_rest.rs):

- `rtw_get_capability_from_ie`, `rtw_get_capability`, `rtw_get_timestampe_from_ie`, `rtw_get_beacon_interval_from_ie`
- `is_same_ess`, `is_same_network`, `rtw_is_same_ibss`, `rtw_generate_random_ibss`

## Notes

- **Pure compare/getter helpers:** `rtw_get_capability_from_ie`, `rtw_get_capability`,
  `rtw_get_timestampe_from_ie`, `rtw_get_beacon_interval_from_ie`, `is_same_ess`,
  `is_same_network`.
- **`rtw_is_same_ibss` is adapter-coupled** — compares `adapter->securitypriv.dot11PrivacyAlgrthm`
  against `pnetwork->network.Privacy`; L2 needs a minimal adapter/securitypriv fixture (same
  pattern as W3-40), not just IE/BSSID byte vectors.
- Scan/join/roam state machines stay in C.
- `rtw_generate_random_ibss` needs deterministic `rtw_random32` stub for L2.
- L2: new `tests/host/mlme/` — BSSID/IE getters, same-ESS/network pairs, adapter fixture for
  `rtw_is_same_ibss` (incl. hidden-AP cases).

## Acceptance

- L0 build + L2 host unit tests for BSSID getters and same-network compare
