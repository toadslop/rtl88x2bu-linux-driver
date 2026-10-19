---
title: "[W4-42] Translate hal_com.c — dump_TX_FIFO WoW debug"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-42
epic: E06
blocked_by: [W4-41]
estimate_loc: 160
---

## Goal

Port `dump_TX_FIFO` from [`hal/hal_com.c`](../../../hal/hal_com.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- Parameter validation (`page_num`, `page_size` 128–512)
- Register peek/poke sequence on `0x106`, `0x209`, `0x140` / `0x144` / `0x148`
- Page dump loop via `rtw_read8` / `rtw_read32` / `rtw_write8` / `rtw_write32` FFI

## Notes

- Debug-only WoW offload helper; gated behind the same `#ifdef` as in C (`CONFIG_WOWLAN` / debug build — preserve Kbuild `cfg`).
- Chains hal_com lane after [W4-41](wave4-41-hal-dump-hal-spec.md).
- L2 optional: mocked register backend recording addr/val sequence for one page.

## Acceptance

- L0 with WoW/debug Kconfig matching pinned 8822BU profile
- L1 symbols for `dump_TX_FIFO`
- No behavior change vs C for invalid-parameter early returns
