---
title: "[W3-118] odm phydm ability and IC init"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-118
epic: E05
blocked_by: [W3-117]
estimate_loc: 190
---

## Goal

Port helpers from [`core/rtw_odm.c`](../../../core/rtw_odm.c) to [`rust/rtw_odm.rs`](../../../rust/rtw_odm.rs):

- `rtw_phydm_ability_ops`
- `rtw_odm_init_ic_type`

## Notes

- PHYDM ability bitmask ops and IC-type init; starts tranche 7 on `rtw_odm.c`.
- Adaptivity and radar-detect helpers ship in W3-119/W3-120.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for odm phydm ability and IC init helpers
