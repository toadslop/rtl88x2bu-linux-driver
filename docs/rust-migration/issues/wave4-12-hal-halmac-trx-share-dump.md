---
title: "[W4-12] Translate hal_halmac.c — TRX share mode dump"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-12
epic: E06
blocked_by: [W4-10]
estimate_loc: 200
---

## Goal

Extend [`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs) with TRX FIFO share-mode
helpers from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c) (`CONFIG_SUPPORT_TRX_SHARED`):

- `_trx_share_mode_drv2halmac`
- `dump_trx_share_mode`

## Notes

- Same-file follow-up to W4-10; `dump_trx_share_mode` is read-only formatting of
  `registry_priv.trx_share_mode`.
- Guard Rust/C wiring with the same `CONFIG_SUPPORT_TRX_SHARED` Kconfig as the C build.
- **Dependency rationale:** shares `rust/hal_halmac.rs` with W4-10 (parallel with W4-11).

## Acceptance

- L0 + L1 for mapper + dump when `CONFIG_SUPPORT_TRX_SHARED` is enabled in the pinned build
- L2 golden strings for registry modes 0–3 and an out-of-range value
