---
title: "[W3-145] Translate rtw_debug_rest.c — proc mlme/rf leaf getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-145
epic: E05
blocked_by: [W3-144]
estimate_loc: 180
---

## Goal

Add [`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) (Kbuild object when `CONFIG_RUST` and
`CONFIG_RUST_RTW_DEBUG`) and port proc seq_file getters from
[`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) under `CONFIG_RTW_DEBUG` /
`CONFIG_PROC_DEBUG` with C swap guards
`#if !defined(CONFIG_RUST) || !defined(CONFIG_RUST_RTW_DEBUG)` (same TU / lane as
[W3-143](wave3-143-debug-mac-bb-reg-dump.md) / [W3-144](wave3-144-debug-adapters-status.md)):

- `proc_get_fwstate`
- `proc_get_mlmext_state`
- `proc_get_qos_option`
- `proc_get_ht_option` (`CONFIG_80211N_HT`)
- `proc_get_rf_info`

## Notes

- First procfs slice in `rtw_debug_proc.rs` — keep handlers as thin `RTW_PRINT_SEL` formatters; proc
  registration stays in C (`os_dep/linux/rtw_proc.c`).
- Wire Makefile `rust/rtw_debug_proc.o` alongside existing `rust/rtw_debug_*.o` under the
  `_rust_cfg_rtw_debug` block (same pattern as W3-143/144).
- Stack after [W3-144](wave3-144-debug-adapters-status.md) on `rtw_debug_rest.c` guard/Makefile lane
  — do not parallelize edits in that TU with W3-143/144 follow-ups.
- C file disjoint from hal_usb / hal_com issues in the same batch.

## Acceptance

- L0 with `CONFIG_PROC_DEBUG=y` on pinned profile
- L1 symbol parity on `proc_get_*` handlers in the swapped object (module-local globals wired from
  procfs, not `EXPORT_SYMBOL`); use an allowlist if bodies move to `rtw_debug_proc.o`
- Proc entry wiring unchanged; behavior matches C output strings
