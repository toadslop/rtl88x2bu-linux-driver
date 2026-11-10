---
title: "[FX-11] Use wrapping arithmetic where C relied on modular wrap (pwrctrl wakeup/lps_tp, recv bh dbg)"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-11
epic: E14
blocked_by: []
estimate_loc: 60
---

## Priority

**P3 (low)** — each site panics (kernel BUG) under `CONFIG_RUST_OVERFLOW_CHECKS=y`
only in non-default configs or on 32-bit Rust kernels, but a panic in an
`extern "C"` fn is never acceptable.

## Problem

| Site | Rust | C original | Reachable |
|---|---|---|---|
| [`rust/rtw_pwrctrl_wakeup.rs`](../../../rust/rtw_pwrctrl_wakeup.rs) :181, :192 (0a29c879 #1642) | `now + ms_to_systime(ms)` | `rtw_get_current_time() + rtw_ms_to_systime(ms)` (wraps) | 32-bit Rust kernels: jiffies start at −300·HZ, so `+` overflows ~5 min after boot (all 6.12 Rust arches are 64-bit) |
| [`rust/rtw_pwrctrl_lps_tp.rs`](../../../rust/rtw_pwrctrl_lps_tp.rs) :228, :251 (29bd53bb #1520, cbd8b6db #1521) | `tx_bytes - acc` / `rx_bytes - acc` (two separate shim reads) | u64 subtraction, wraps | `CONFIG_LPS_CHK_BY_TP`: `acc_*_bytes` snapshot from cmd thread can land between the two reads → underflow → oops |
| [`rust/rtw_recv_bh_dbg.rs`](../../../rust/rtw_recv_bh_dbg.rs) :326, :345 (d4591f1d #1538) | `dq_cnt + 1`, `(p - base) as U32` | `cnt++`, pointer diff | `DBG_RX_BH_TRACKING=1` (default 0) |

## Fix

- `wrapping_add` for systime sums (keep `time_after` semantics).
- `wrapping_sub` for the byte-counter deltas.
- `wrapping_add(1)` for the counter; `(p as usize).wrapping_sub(base as usize) as u32`.
- Same file: `dump_rx_bh_tk` formats raw pointers as hex (f2518ff2 #1512) where C used
  `%p` (hashed); pass pointers to a C `%p` print shim to avoid leaking addresses.

## Acceptance

- L0; L1 unchanged.
- `llvm-objdump -dr` of the three objects: no `panic_const_add_overflow` /
  `panic_const_sub_overflow` reachable from the listed functions.
- L2 wakeup / lps_tp vectors with values at `u64::MAX` / `c_ulong::MAX` boundaries.
