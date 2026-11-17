---
title: "[W4-112] Translate hal_dm.c — PHYDM efuse trim read hooks"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-112
epic: E06
blocked_by: []
estimate_loc: 40
---

## Goal

Port `rtw_phydm_read_efuse` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — call `phydm_get_thermal_trim_offset` and
`phydm_get_power_trim_offset` on the adapter PHYDM context.

## Notes

- **Parallel lane:** single-export leaf; no efuse MMIO in Rust (PHYDM API only).

## Acceptance

- L0 + L1 symbol parity
- L2: mock PHYDM records both trim query calls

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)