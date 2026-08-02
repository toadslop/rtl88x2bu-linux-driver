---
title: "[W3-128] beamforming entry lookup and packet leaf"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-128
epic: E05
blocked_by: [W3-127]
estimate_loc: 220
---

## Goal

Port helpers from [`core/rtw_beamforming.c`](../../../core/rtw_beamforming.c) to [`rust/rtw_beamforming.rs`](../../../rust/rtw_beamforming.rs):

- `rtw_bf_bfee_get_entry_cap_by_macid`
- `rtw_bf_bfer_get_entry_by_addr`
- `rtw_bf_bfee_get_entry_by_addr`
- `rtw_bf_get_ndpa_packet`
- `rtw_bf_get_report_packet`

## Notes

- Exported beamforming entry lookup and NDPA/report packet helpers; starts tranche 7 on `rtw_beamforming.c`.
- Internal sounding state machine (`_sounding_*`, `_bfer_*`, `_bfee_*`) stays in C until later tranches.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for beamforming entry lookup and packet leaf helpers
