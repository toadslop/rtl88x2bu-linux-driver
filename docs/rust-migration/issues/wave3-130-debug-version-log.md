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

- **Scaffold owner:** introduces `rust/rtw_debug.rs` and Kbuild/Makefile wiring.
  `core/rtw_debug.c` is ~8k lines as a single `core/rtw_debug.o` today — before the
  CONFIG_RUST object swap, extract all unported symbols to `core/rtw_debug_rest.c`
  (same `*_rest.c` pattern as `wave3-104` / `rtw_br_ext_rest.c`). Only then replace
  `core/rtw_debug.o` with `rust/rtw_debug.o` for the symbols in this slice.
  Later `W3-131`…`W3-138` / `W3-132`…`W3-134` add functions into the Rust object
  while the remainder stays in `rtw_debug_rest.c` until fully ported.
- `dump_log_level` is gated on `CONFIG_RTW_DEBUG`; preserve `#ifdef` behavior via
  Kbuild `cfg` or thin C stubs as in other debug ports.
- Procfs/`seq_file` integration stays in C; Rust owns formatting logic callable from C.
- L2 optional: snapshot tests via a host shim printing into a buffer (if trivial);
  otherwise L0 + L1 only for this slice.

## Acceptance

- `rtw_debug_rest.c` holds every `rtw_debug.c` symbol **not** moved to Rust in this slice
- L0 build (`make KDIR=/opt/linux LLVM=1`) + L1 symbol check on swapped objects
- `extern "C"` symbols and signatures unchanged for proc/debug callers
