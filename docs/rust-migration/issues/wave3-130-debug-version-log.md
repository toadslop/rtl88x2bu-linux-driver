---
title: "[W3-130] Translate rtw_debug.c — driver version and log level dumps"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-130
epic: E05
blocked_by: []
estimate_loc: 200
---

## Goal

Port debug print helpers from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs):

- `dump_drv_version`
- `dump_log_level`

## Notes

- `dump_log_level` is gated on `CONFIG_RTW_DEBUG`; preserve `#ifdef` behavior via
  Kbuild `cfg` or thin C stubs as in other debug ports.
- Procfs/`seq_file` integration stays in C; Rust owns formatting logic callable from C.
- L2 optional: snapshot tests via a host shim printing into a buffer (if trivial);
  otherwise L0 + L1 only for this slice.

## Acceptance

- L0 build (`make KDIR=/opt/linux LLVM=1`) + L1 symbol check on swapped objects
- `extern "C"` symbols and signatures unchanged for proc/debug callers
