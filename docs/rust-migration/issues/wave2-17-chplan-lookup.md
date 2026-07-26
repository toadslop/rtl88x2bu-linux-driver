---
title: "[W2-17] Translate rtw_chplan lookup helpers"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-17
epic: E04
blocked_by: [W2-16, T4]
estimate_loc: 200
---

## Goal

Port pure channel-plan lookup getters from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c) to [`rust/rtw_chplan.rs`](../../../rust/rtw_chplan.rs):

- `rtw_chplan_get_default_regd_{2g,5g}`, `rtw_chplan_get_default_regd`
- `rtw_chplan_is_empty`, `rtw_is_channel_plan_valid`

## Table strategy (canonical)

**Static tables stay in C** (`RTW_ChannelPlanMap`, `rtw_channel_def_2g` / `rtw_channel_def_5g`, …). Rust reads them via `extern "C"` until a future dedicated data-migration issue. Do **not** duplicate ~300+ lines of table data in this slice.

## Notes

- Introduce `ChannelPlanId` / `RegulatoryDomain` from A2 at the Rust API; `extern "C"` shims preserve `u8` signatures.
- L2 via T4 (`tests/host/chplan/`): freeze oracle vectors for valid/invalid plan ids and regd combinations.
- In-flight: `cursor/w2-17a-chplan-harness-3dd4` (T4 harness), `cursor/w2-17b-chplan-lookup-rust-3dd4` (getter port).

## Acceptance

- L0 build + L1 symbols + L2 host chplan vectors (T4)
- Remaining `rtw_chplan.c` functions stay in C until W2-18…W2-20
