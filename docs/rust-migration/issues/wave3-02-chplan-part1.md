---
title: "[W3-02] Translate rtw_chplan.c part 1 — tables + getters"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-02
epic: E05
blocked_by: [A2, T4]
estimate_loc: 200
---

## Goal

Port the static channel-plan data and pure getters from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c):

- `RTW_ChannelPlanMap` / `RTW_ChannelPlanMap_size`
- `rtw_channel_def_2g` / `rtw_channel_def_5g` tables (as needed by getters)
- `rtw_chplan_get_default_regd_{2g,5g}`, `rtw_chplan_get_default_regd`
- `rtw_chplan_is_empty`, `rtw_is_channel_plan_valid`

Extract remaining C to `rtw_chplan_rest.c` per [architecture.md](../architecture.md) multi-part rules.

## Notes

- Introduce domain types from A2 (`ChannelPlanId`, `RegulatoryDomain`) at the Rust API; `extern "C"` shims preserve `u8` signatures.
- L2 via T4 host harness: freeze oracle vectors for every valid/invalid plan id and regd combination.

## Acceptance

- L0 build + L1 symbols + L2 host chplan vectors (T4)
- Remaining `rtw_chplan.c` functions stay in `rtw_chplan_rest.c` until later W3 issues
