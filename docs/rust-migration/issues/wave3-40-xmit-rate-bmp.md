---
title: "[W3-40] Translate rtw_xmit.c — tx rate bitmap helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-40
epic: E05
blocked_by: [W3-39]
estimate_loc: 200
---

## Goal

Port tx rate bitmap helpers from [`core/rtw_xmit.c`](../../../core/rtw_xmit.c) to [`rust/rtw_xmit.rs`](../../../rust/rtw_xmit.rs):

- `rtw_get_tx_bw_mode`
- `rtw_get_adapter_tx_rate_bmp_by_bw`, `rtw_get_shared_macid_tx_rate_bmp_by_bw`
- `rtw_get_tx_bw_bmp_of_ht_rate`, `rtw_get_tx_bw_bmp_of_vht_rate`

## Notes

- These helpers read `_adapter` / `dvobj` / `sta_info` / `macid_ctl` state (e.g.
  `rtw_get_tx_bw_mode` uses `sta->cmn.bw_mode`, `adapter->mlmeextpriv`, and
  `ADAPTER_TX_BW_*`; rate-bmp helpers iterate `macid_ctl` via `adapter_to_dvobj()`).
  Expect bindgen + populated adapter fixtures or thin C shims for L2 (as W3-34 does
  for `padapter`); `size/~200` includes harness scaffolding.
- Frame queue management and HAL xmit paths stay in C.
- L2: host harness under `tests/host/xmit/` with rate/bw bitmap vectors.

## Acceptance

- L0 build + L2 host unit tests for tx rate bitmap helpers (adapter/shim
  requirements documented in harness)
