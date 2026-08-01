---
title: "[W3-49] Translate rtw_xmit.c — aggregate rate bitmap and RA short GI"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-49
epic: E05
blocked_by: [W3-48]
estimate_loc: 200
---

## Goal

Port aggregate tx rate bitmap helpers from [`core/rtw_xmit.c`](../../../core/rtw_xmit.c) to [`rust/rtw_xmit.rs`](../../../rust/rtw_xmit.rs):

- `rtw_get_adapter_tx_rate_bmp`, `rtw_update_tx_rate_bmp`, `query_ra_short_GI`

## Notes

- Reads `_adapter` / `dvobj` / `macid_ctl` state (same fixture pattern as W3-40).
- HAL xmit and queue management stay in C.
- L2: extend `tests/host/xmit/` with adapter/macid_ctl rate bitmap vectors.

## Acceptance

- L0 build + L2 host unit tests for aggregate rate bitmap and RA short GI helpers
