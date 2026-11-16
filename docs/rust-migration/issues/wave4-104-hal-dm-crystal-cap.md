---
title: "[W4-104] Translate hal_dm.c — crystal cap register setter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-104
epic: E06
blocked_by: []
estimate_loc: 60
---

## Goal

Port `rtw_phydm_set_crystal_cap` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — delegate to `phydm_set_crystal_cap_reg`.

## Notes

- **Parallel lane:** single-export leaf; ideal follow-up once larger hal_dm stacks land.
- Return value mirrors C `bool` success from PHYDM API.

## Acceptance

- L0 + L1 symbol parity
- L2: mock `dm_struct` records cap argument forwarded to PHYDM stub

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)