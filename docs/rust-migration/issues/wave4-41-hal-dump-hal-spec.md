---
title: "[W4-41] Translate hal_com.c — dump_hal_spec formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-41
epic: E06
blocked_by: [W4-40]
estimate_loc: 200
---

## Goal

Port `dump_hal_spec` and its cap-string tables from [`hal/hal_com.c`](../../../hal/hal_com.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- Static `_band_cap_str`, `_bw_cap_str`, `_proto_cap_str`, `_wl_func_str` tables
- Field-by-field `RTW_PRINT_SEL` lines for `struct hal_spec_t` (macid, sec, RF path, NSS, caps, txgi, wl_func, tx AC lifetime, pg txpwr)

## Notes

- Same hal_com debug lane as [W4-39](wave4-39-hal-com-tx-aclt-force-dump.md) / [W4-40](wave4-40-hal-com-tx-aclt-confs-dump.md); reuse existing `hal_com` FFI for `GET_HAL_SPEC` / print macros.
- Does **not** port the `hal_chk_*` / `hal_is_*` inline helpers immediately below `dump_hal_spec` in C (separate slice if needed).
- L2: host golden dump from a synthetic `hal_spec_t` fixture (cap bit combinations).

## Acceptance

- L0 build (`make KDIR=/opt/linux LLVM=1`)
- L1 symbol check for `dump_hal_spec`
- L2: golden output matches C for at least 2G+5G and 11ac proto cap mixes
