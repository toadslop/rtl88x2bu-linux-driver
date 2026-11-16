---
title: "[W4-103] Translate hal_dm.c — runtime TRX path decision (8822B)"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-103
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `rtw_hal_runtime_trx_path_decision` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) for the **8822B / 8822C / 8192F** branch
used by this driver profile — `txpath`/`rxpath` validation, `txpath_nss` derivation,
and `HalData` field updates.

## Notes

- **Parallel lane:** TRX/NSS tables only — does not port `rtw_phydm_init` or `rtw_phydm_config_trx_path`.
- Chip `#if` blocks for non-8822B families may remain C stubs or `unreachable!` in kernel build as today.

## Acceptance

- L0 + L1 under default 8822B USB kconfig
- L2: host vectors for `max_tx_cnt`, `path_div`, and `tx_npath` registry combinations → `txpath` / `txpath_nss[0]`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)