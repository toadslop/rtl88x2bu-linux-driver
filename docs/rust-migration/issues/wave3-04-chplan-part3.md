---
title: "[W3-04] Translate rtw_chplan.c part 3 — country lookup"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-04
epic: E05
blocked_by: [W3-02]
estimate_loc: 200
---

## Goal

Port country-code → channel-plan lookup from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c):

- `country_chplan_map` (and `CUSTOMIZED_` / `RTW_DEF_MODULE_REGULATORY_CERT` variants as compiled for 8822B USB default config)
- `rtw_get_chplan_from_country`
- `rtw_def_module_country_chplan_map` (if enabled)

## Notes

- Large static table; Rust `const` arrays are fine. Use `CountryCode` domain type (A2) internally.
- L2: spot-check known countries (US, JP, DE, …), invalid codes, case normalization (`alpha_to_upper` behavior).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T4)
