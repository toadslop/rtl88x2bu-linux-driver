---
title: "[W4-31] Translate hal_dm.c — dump_sta_info debug formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-31
epic: E06
blocked_by: [W4-30]
estimate_loc: 200
---

## Goal

Port `dump_sta_info` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- Format STA RA / RSSI / rate / bandwidth lines via `RTW_PRINT_SEL`
- Call existing C shims for `ch_width_str`, `HDATA_RATE`, `rtw_get_current_tx_rate`, and `rtw_get_current_tx_sgi`
- Null `psta` early return unchanged

## Notes

- Debug-only PHYDM-adjacent dump — no register I/O.
- **Dependency rationale:** shares `hal_dm` Rust module and FFI glue from W4-29/W4-30.

## Acceptance

- L0 + L1 for `dump_sta_info`
- L2: capture `RTW_PRINT_SEL` output against a fixture `sta_info` + stub rate/SGI helpers
