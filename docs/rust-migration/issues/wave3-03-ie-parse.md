---
title: "[W3-03] Translate rtw_ieee80211 IE parse helpers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-03
epic: E05
blocked_by: [W3-02]
estimate_loc: 200
---

## Goal

Port pure IE parse helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211.rs`](../../../rust/rtw_ieee80211.rs):

- `rtw_get_ie`, `rtw_get_ie_ex`, `rtw_ies_remove_ie`

## Notes

- Low HAL coupling; good early Wave 3 leaf alongside swcrypto.
- L2 via T5 (`tests/host/ie/`): freeze vectors from C oracle before port.
- In-flight: `cursor/w3-03-ie-parse-3dd4`.

## Acceptance

- L0 build + L1 symbols + L2 host IE vectors
