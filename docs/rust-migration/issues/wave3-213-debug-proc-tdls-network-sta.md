---
title: "[W3-213] Translate rtw_debug_rest.c — proc TDLS network + STA display"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-213
epic: E05
blocked_by: [W3-212]
estimate_loc: 200
---

## Goal

Port remaining TDLS debug display helpers from [`core/rtw_debug_rest.c`](../../../core/rtw_debug_rest.c) into
[`rust/rtw_debug_proc_tdls.rs`](../../../rust/rtw_debug_proc_tdls.rs):

- `proc_tdls_display_network_info`
- `proc_tdls_display_tdls_sta_info`

## Notes

- Read-only formatting over `mlme_priv` / `tdlsinfo` — no new TDLS protocol logic in Rust.
- Chains after [W3-212](wave3-212-debug-proc-tdls-enable.md) (shared module + procfs wiring).

## Acceptance

- L0 with `CONFIG_TDLS`
- L1 on both display helpers
- L2: golden subset of AP/GO and linked-STA banner lines for fixture adapter
