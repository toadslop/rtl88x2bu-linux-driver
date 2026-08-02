---
title: "[W3-129] beamforming init and cmd_hdl leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-129
epic: E05
blocked_by: [W3-128]
estimate_loc: 210
---

## Goal

Port helpers from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to [`rust/rtw_beamforming.rs`](../../../rust/rtw_beamforming.rs):

- `rtw_bf_send_vht_gid_mgnt_packet`
- `rtw_bf_get_vht_gid_mgnt_packet`
- `rtw_bf_init`
- `rtw_bf_cmd_hdl`

## Notes

- Beamforming init, GID mgmt, and cmd dispatch leaf helpers.
- Remaining exports (`rtw_bf_cmd`, `rtw_bf_update_attrib`, `rtw_bf_c2h_handler`, `rtw_bf_update_traffic`) stay in C until a later tranche.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for beamforming init and cmd_hdl leaf helpers
