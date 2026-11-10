---
title: "[FX-04] pwrctrl: rtw_set_ps_mode drops TDLS PS null-data notifications"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-04
epic: E14
blocked_by: [FX-01]
estimate_loc: 60
---

## Priority

**P2 (medium)** — `CONFIG_TDLS=y` only (Makefile default `n`, but the Makefile
already wires `--cfg config_tdls`, so TDLS is a supported configuration).

## Problem

C `rtw_set_ps_mode` (fallback in [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c),
`#ifdef CONFIG_TDLS` after the `lps_leave_cnts` / `lps_enter_cnts` bumps) walks
`pstapriv->sta_hash` and calls
`issue_nulldata_to_TDLS_peer_STA(padapter, mac, 0 /* leave */ | 1 /* enter */, 0, 0)`
for every `TDLS_LINKED_STATE` peer.

The Rust port [`rust/rtw_pwrctrl_set_ps.rs`](../../../rust/rtw_pwrctrl_set_ps.rs)
(dd54cd9e #1616, wired by b599ed73 #1617) has no TDLS step and no shim; the C body
is compiled out under `CONFIG_RUST` and `rust/rtw_pwrctrl_set_ps.o` is linked
unconditionally. Linked TDLS peers are never told about PS entry/exit, so they
transmit to a dozing STA (lost frames) or keep buffering after wake.

## Fix

1. C shim `void rtw_rust_set_ps_tdls_nulldata(PADAPTER, u8 pwr_bit)` in
   `core/rtw_pwrctrl.c`, body under `#ifdef CONFIG_TDLS` (empty otherwise),
   copying the original loop verbatim.
2. Rust: call it with `0` right after `SET_PS_U32_LPS_LEAVE` (line ~453) and with
   `1` right after `SET_PS_U32_LPS_ENTER` (line ~486), preserving C order.

## Acceptance

- L0 default build; L0 with `CONFIG_TDLS = y`.
- L1 unchanged for `rust/rtw_pwrctrl_set_ps.o` exports; new import is defined in both configs.
- Review diff of the shim body vs `dd54cd9e^:core/rtw_pwrctrl.c` is identical.

## Notes

- **Dependency rationale:** same C TU as FX-01 (`core/rtw_pwrctrl.c`); chained to avoid merge conflicts, not an API dependency.
- Pre-existing: the C loop walks `sta_hash` without `sta_hash_lock`; keep as-is (parity).
