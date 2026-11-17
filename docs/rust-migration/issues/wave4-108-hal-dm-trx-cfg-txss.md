---
title: "[W4-108] Translate hal_dm.c — TRX cfg for TX SS by throughput"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-108
epic: E06
blocked_by: []
estimate_loc: 80
---

## Goal

Under `CONFIG_CTRL_TXSS_BY_TP`, port `rtw_phydm_trx_cfg` from [`hal/hal_dm.c`](../../../hal/hal_dm.c)
into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — query TRX paths, force 1SS TX when requested,
call `phydm_api_trx_mode`.

## Notes

- **Parallel lane:** runtime path **decision** stays in [W4-103](wave4-103-hal-dm-runtime-trx-path.md); this is the throughput-driven TRX **apply** helper only.
- Preserve 8822B `BB_PATH_A` vs `BB_PATH_AB` behavior.

## Acceptance

- L0 + L1 when `CONFIG_CTRL_TXSS_BY_TP` is enabled
- L2: mock PHYDM records `txpath`/`rxpath`/`txpath_1ss` passed to `phydm_api_trx_mode`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)