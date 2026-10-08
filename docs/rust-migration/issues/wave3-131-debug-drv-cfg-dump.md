---
title: "[W3-131] Translate rtw_debug.c — dump_drv_cfg Kconfig banner"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-131
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port `dump_drv_cfg` from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs) (under `CONFIG_PROC_DEBUG`).

## Notes

- Large conditional compile banner — port verbatim string/format branches; split into
  helper functions in Rust if needed to stay reviewable.
- Reads compile-time `CONFIG_*` values and globals like `rtw_recvbuf_nr`; keep FFI
  reads at the edge.
- Register dump paths (`mac_reg_dump`, etc.) stay in C for later slices.

## Acceptance

- L0 build with `CONFIG_PROC_DEBUG=y` in the pinned kernel config
- L1 symbols for `dump_drv_cfg` match the C object baseline
