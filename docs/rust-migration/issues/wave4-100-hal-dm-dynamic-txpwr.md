---
title: "[W4-100] Translate hal_dm.c — dynamic TX power desc helpers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-100
epic: E06
blocked_by: []
estimate_loc: 140
---

## Goal

Port `CONFIG_SUPPORT_DYNAMIC_TXPWR` helpers from [`hal/hal_dm.c`](../../../hal/hal_dm.c)
into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_phydm_fill_desc_dpt` — chip-specific TX descriptor power offset for 8822B (and stubs for other profiles as in C)
- `rtw_phydm_set_dyntxpwr` — `odm_set_dyntxpwr` delegate

## Notes

- **Parallel lane:** disjoint from SetHalODMVar / IQK work (W4-94…W4-98); hook registration stays in existing `rtw_phydm_ops_func_init` Rust slice.
- Preserve `SET_TX_DESC_TXPWR_OFSET_8822B` semantics; no new MMIO.

## Acceptance

- L0 + L1 when `CONFIG_SUPPORT_DYNAMIC_TXPWR` is enabled for 8822B
- L2: fixed TX desc buffer + mock `dm_struct` records applied offset byte(s)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)