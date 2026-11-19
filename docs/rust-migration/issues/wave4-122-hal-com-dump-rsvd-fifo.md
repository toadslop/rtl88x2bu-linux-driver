---
title: "[W4-122] Translate hal_com_rest.c — RSVD page and FIFO debug dumps"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-122
epic: E06
blocked_by: [W4-121]
estimate_loc: 150
---

## Goal

Extend `rust/hal_com_rsvd_page.rs` with debug formatters from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c):

- `rtw_dump_rsvd_page` (`CONFIG_RTW_DEBUG` / proc paths)
- `rtw_dump_fifo` when `CONFIG_SUPPORT_FIFO_DUMP`

## Notes

- **Same lane as W4-121** — shared `hal_com_rsvd_page.rs`; `rtw_dump_rsvd_page` (`CONFIG_RTW_DEBUG` / proc) calls W4-121's `rtw_hal_get_rsvd_page` and `HAL_DEF_TX_PAGE_SIZE`.
- **`rtw_dump_fifo`** (`CONFIG_SUPPORT_FIFO_DUMP`) does **not** call `rtw_hal_get_rsvd_page`; it invokes `rtw_halmac_dump_fifo(fifo_sel, fifo_addr, RND4(fifo_size), buffer)` directly. L2 FIFO path must stub `rtw_halmac_dump_fifo`, not only `get_rsvd_page`.
- Output uses existing `rtw_rust_*` print_sel shims for L2 string compare.

## Acceptance

- L0 + L1 for both dump entry points (under their Kconfig gates)
- L2: golden sel output for synthetic page_size / stub fifo read

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-121 (`rtw_hal_get_rsvd_page`)