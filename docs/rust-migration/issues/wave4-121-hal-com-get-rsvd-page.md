---
title: "[W4-121] Translate hal_com_rest.c — HALMAC reserved page fetch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-121
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Port `rtw_hal_get_rsvd_page` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_rsvd_page.rs` (RTW_HALMAC path: `rtw_halmac_dump_fifo` for FIFO sel RSVD_PAGE).

## Notes

- **Parallel lane:** FIFO dump helper — no overlap with MBSSID CAM or `hal_dm.rs` in-flight work.
- Non-HALMAC register bit-bang loop may remain in C behind `RTW_HALMAC` cfg for this slice.

## Acceptance

- L0 on pinned 8822BU + HALMAC profile
- L1 symbol `rtw_hal_get_rsvd_page`
- L2: stub `rtw_halmac_dump_fifo` records addr/len/buffer fill vs C oracle

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)