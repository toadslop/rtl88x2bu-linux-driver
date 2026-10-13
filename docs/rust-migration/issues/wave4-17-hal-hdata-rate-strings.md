---
title: "[W4-17] Translate hal_com_rest.c — HDATA_RATE descriptor strings"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-17
epic: E06
blocked_by: [W4-16]
estimate_loc: 200
---

## Goal

Port hardware descriptor rate name table from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `_HDATA_RATE` (static table)
- `HDATA_RATE` / `rtw_rust_hal_com_hdata_rate` lookup used by rate dumps and L2 oracles

## Notes

- Table-only; replaces C strings still referenced from W4-01 `hw_rate_to_m_rate` dump paths.
- Drop duplicate C table from `hal_com_rest.c` on swap; keep `HOST_HAL_COM_HW_RATE_TEST` oracle shim until L2 vectors pass.
- Same `hal_com` lane — chain after W4-16.

## Acceptance

- L0 + L1 for `_HDATA_RATE` and `rtw_rust_hal_com_hdata_rate` (the linkable symbols;
  `HDATA_RATE(rate)` in `include/hal_com.h` is a macro over `_HDATA_RATE`, not an L1 export)
- `hal_com.h` keeps callers working: macro points at the Rust-exported table or a thin C shim
- L2: enumerate `DESC_RATE*` indices including MCS/VHT edges and `"UNKNOWN"` fallback
