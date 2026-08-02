---
title: "[W3-115] sdio cmd52/53 I/O wrappers"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-115
epic: E05
blocked_by: [W3-114]
estimate_loc: 160
---

## Goal

Port helpers from [`core/rtw_sdio.c`](../../../core/rtw_sdio.c) to [`rust/rtw_sdio.rs`](../../../rust/rtw_sdio.rs):

- `rtw_sdio_read_cmd52`
- `rtw_sdio_read_cmd53`
- `rtw_sdio_write_cmd52`
- `rtw_sdio_write_cmd53`
- `rtw_sdio_f0_read`

## Notes

- Thin SDIO I/O wrappers over `sdio_io`; starts tranche 7 on `rtw_sdio.c`.
- Static `sdio_io` stays in C; Rust ports the exported cmd52/53 entry points.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for sdio cmd52/53 I/O wrappers
