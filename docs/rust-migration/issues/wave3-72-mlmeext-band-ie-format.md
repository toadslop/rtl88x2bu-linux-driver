---
title: "[W3-72] Translate rtw_mlme_ext.c — band-change beacon IE update"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-72
epic: E05
blocked_by: [W3-71]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_mlme_ext.c`](../../../core/rtw_mlme_ext.c) to [`rust/rtw_mlme_ext_rest.rs`](../../../rust/rtw_mlme_ext_rest.rs):

- `change_band_update_ie`

## Notes

- **`CONFIG_AP_MODE` only** — updates ERP/VHT IEs, supported rates, and
  `WLAN_BSSID_EX.Length` when AP channel band changes (2.4 vs 5 GHz).
- Mgnt frame attribute builders are W3-68 (`update_mgntframe_attrib`, etc.); there
  is no `format` function in this file (the SEC CAM comment at ~15313 is unrelated).
- L2: new `tests/host/mlme_ext/` harness with JSON differential vectors for 2.4/5G
  band-change IE snapshots (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for band-change beacon IE update
