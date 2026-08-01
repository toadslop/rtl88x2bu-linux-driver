---
title: "[W3-41] Translate rtw_ieee80211.c — rate-section and ch-offset mapping"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-41
epic: E05
blocked_by: [W3-40]
estimate_loc: 200
---

## Goal

Port rate-section and channel-offset mapping helpers from [`core/rtw_ieee80211.c`](../../../core/rtw_ieee80211.c) to [`rust/rtw_ieee80211_rest.rs`](../../../rust/rtw_ieee80211_rest.rs):

- `mgn_rate_to_rs`, `rtw_get_cckrate_size`, `rtw_is_cckrates_included`, `rtw_is_cckratesonly_included`, `rtw_get_rateset_len`
- `secondary_ch_offset_to_hal_ch_offset`, `hal_ch_offset_to_secondary_ch_offset`

## Notes

- Pure table/logic helpers; no adapter state beyond rate bytes and offset enums.
- WPA/RSN/chbw/frame helpers already ported in W3-26…W3-32; this slice fills the rate-section gap.
- L2: extend `tests/host/ie/` with rate-byte and secondary-offset oracle vectors.

## Acceptance

- L0 build + L2 host unit tests for rate-section and ch-offset mapping
