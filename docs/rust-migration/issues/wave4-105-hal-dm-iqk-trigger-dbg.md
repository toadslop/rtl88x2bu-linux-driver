---
title: "[W4-105] Translate hal_dm.c — IQK trigger debug wrapper"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-105
epic: E06
blocked_by: []
estimate_loc: 50
---

## Goal

Port `rtw_phydm_iqk_trigger_dbg` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — delegate to `halrf_segment_iqk_trigger` or
`halrf_iqk_trigger` per chip profile (8822B branch).

## Notes

- **Parallel lane:** complements [W4-98](wave4-98-hal-dm-iqk-lck-trigger.md); does not port manufacturing test wrappers.
- No new PS-deny orchestration in this slice.

## Acceptance

- L0 + L1 symbol parity under 8822B USB kconfig
- L2: mock `dm_struct` records which `halrf_*` entry fired for `recovery`/`clear`/`segment` tuples

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)