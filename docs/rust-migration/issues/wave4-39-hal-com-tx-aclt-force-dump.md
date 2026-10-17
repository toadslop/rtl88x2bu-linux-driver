---
title: "[W4-39] Translate hal_com.c — dump_tx_aclt_force_val formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-39
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `dump_tx_aclt_force_val` from [`hal/hal_com.c`](../../../hal/hal_com.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- Unit/maximum us lines from `hal_spec->tx_aclt_unit_factor`
- Table header and numeric row for `tx_aclt_force_val`
- AUTO/FORCE summary line (`TX_ACLT_FORCE_MSG_LEN` snprintf semantics)

## Notes

- Independent **hal_com** debug lane; FFI reads `dvobj->tx_aclt_force_val` and primary adapter `hal_spec` via thin C stubs (same pattern as other hal_com dumps).
- Does not port `rtw_hal_set_tx_aclt_force_val` (separate issue if needed).

## Acceptance

- L0 + L1 for `dump_tx_aclt_force_val`
- L2: golden sel-buffer output for AUTO and FORCE fixture configs
