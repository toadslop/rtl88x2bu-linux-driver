---
title: "[W3-145] Translate rtw_debug_rest.c — proc mlme/rf leaf getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-145
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Add [`rust/rtw_debug_proc.rs`](../../../rust/rtw_debug_proc.rs) (Kbuild + `rtw_debug_rest.c` split) and port proc seq_file getters from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c):

- `proc_get_fwstate`
- `proc_get_mlmext_state`
- `proc_get_qos_option`
- `proc_get_ht_option` (`CONFIG_80211N_HT`)
- `proc_get_rf_info`

## Notes

- First procfs slice — keep handlers as thin `RTW_PRINT_SEL` formatters; proc registration stays in C.
- Parallel debug lane (no overlap with hal_usb / hal_com issues in the same batch).

## Acceptance

- L0 + L1 for exported `proc_get_*` symbols
- Proc entry wiring unchanged; behavior matches C output strings
