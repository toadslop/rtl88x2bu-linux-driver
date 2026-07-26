---
title: "[W2-19] Translate rtw_chplan country lookup"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-19
epic: E04
blocked_by: [W2-17, A2]
estimate_loc: 200
---

## Goal

Port country-code → channel-plan lookup from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c) to [`rust/rtw_chplan.rs`](../../../rust/rtw_chplan.rs):

- `rtw_get_chplan_from_country` (logic only; table stays in C)
- Generic `country_chplan_map` (~250 lines) — accessed via `extern`, not re-copied into Rust

## 8822BU scope

Default `88x2bu` build has `RTW_DEF_MODULE_REGULATORY_CERT=0` ([`include/drv_conf.h`](../../../include/drv_conf.h)), so the **generic** `country_chplan_map` path is in scope. The `RTW_DEF_MODULE_REGULATORY_CERT` variant tables (lines ~633–2122) and `rtw_def_module_country_chplan_map()` are **out of scope** for this driver unless a future build flag enables them.

## Table strategy

Same as W2-17: `country_chplan_map` remains a C `static const` array; Rust calls through `extern "C"`. Use `CountryCode` (A2) internally at the Rust API.

## Notes

- L2: spot-check known countries (US, JP, DE, …), invalid codes, case normalization (`alpha_to_upper` behavior).
- In-flight: `cursor/w2-19-chplan-country-rust-3dd4`.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T4)
