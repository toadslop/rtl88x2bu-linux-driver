---
title: "[FX-13] debug procs: setter/print parity (ips_mode string, %d parsing, warn/errno)"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-13
epic: E14
blocked_by: [FX-08]
estimate_loc: 90
---

## Priority

**P3 (low)** — user-visible proc behaviour changes in the default build.

## Problem

1. **`ps_info` invalid IPS mode** (4b1636a9 #1785): C printed `"invalid ips_mode"`;
   now `snprintf(…, "%d", ips_mode)` in `core/rtw_debug.c` (~2764) and the fallback in
   `core/rtw_debug_rest.c`. Reachable with `rtw_ips_mode=5`. (The same commit's
   `sprintf` into a string literal → static buffer fix for LPS/WOW LPS is correct; keep it.)
2. **AMPDU setters parse `%u`** (4ea110dd #1697, b59ee137 #1702):
   [`rust/rtw_debug_proc_ampdu.rs`](../../../rust/rtw_debug_proc_ampdu.rs) (~196, ~518) uses
   `rtw_rust_debug_proc_sscanf_u32` (`"%u"`); C used `"%d "`. The kernel `vsscanf`
   stops at `-` for `%u`, so writing `-1` (stored as 0xFF = auto) to
   `rx_ampdu_factor`, `rx_ampdu_density`, `tx_ampdu_density`, `tx_quick_addba_req`
   is now ignored. The `RTW_INFO("padapter->driver_rx_ampdu_factor = %x")` lines are gone.
3. **Oversize writes**: Rust returns `-EFAULT` for `count > 32` before C, so the
   original `rtw_warn_on(1)` no longer fires (ampdu.rs, pci.rs).
4. **Return codes**: monitor / simple_config / xmit_block setters (be0eaf49 #1788,
   `core/rtw_debug.c` ~4726/4777/4809) now return `-EFAULT` on NULL buffer or failed
   `copy_from_user` where C returned `count`.

## Fix

Restore the literal; add a `%d` sscanf helper and use it where C used `%d`; restore
the `RTW_INFO` lines and the `rtw_warn_on` (via C shim); match C return values.

## Acceptance

- L0; L1 unchanged.
- L2 debug-proc vectors: `-1` input stores 0xFF; `count = 33` triggers the warn shim;
  `ps_info` with `ips_mode_req = 5` prints `invalid ips_mode`.

## Notes

- **Dependency rationale:** same files as FX-08 (`core/rtw_debug.c`, `core/rtw_debug_rest.c`, `rust/rtw_debug_proc_*.rs`).
