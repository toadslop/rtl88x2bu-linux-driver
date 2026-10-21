---
title: "[W3-151] Translate rtw_debug_rest.c — proc scan param + abort"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-151
epic: E05
blocked_by: [W3-150]
estimate_loc: 150
---

## Goal

Port sitesurvey proc helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs):

- `proc_get_scan_param` / `proc_set_scan_param` — `ss_res` fields with `CONFIG_80211N_HT` / `CONFIG_SCAN_BACKOP` columns
- `proc_get_scan_abort` — `rtw_scan_abort_timeout` wrapper

## Notes

- Mirror C preprocessor column macros for HT and backop optional fields in Rust `cfg` attributes.
- Chains after [W3-150](wave3-150-debug-proc-rx-tx-stat.md).

## Acceptance

- L0 on pinned profile (HT + scan backop flags as in CI kernel config)
- L1 on all three proc symbols
- `proc_set_scan_param` partial `sscanf` field updates match C
