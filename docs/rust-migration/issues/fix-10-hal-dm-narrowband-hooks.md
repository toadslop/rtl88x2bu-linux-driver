---
title: "[FX-10] hal_dm: narrowband else now skips all four phydm hooks"
labels: [rust-migration, phase-1, regression, size/~100]
type: child
id: FX-10
epic: E14
blocked_by: []
estimate_loc: 30
---

## Priority

**P3 (low/medium)** — only with `CONFIG_NARROWBAND_SUPPORTING` (commented out in
`include/hal_ic_cfg.h`), but then phydm dereferences NULL.

## Problem

Original C (`19014834^:hal/hal_dm.c` ~496–508): the dangling `else` after the
narrowband `if` covered only the `ODM_CMNINFO_BAND` hook; SEC_MODE, BW and
NET_CLOSED were always hooked.

Now ([`hal/hal_dm.c`](../../../hal/hal_dm.c) ~500–516, 19014834 W4-38 PR2 #1363) the
`else` covers one call, `rtw_rust_hal_dm_init_odm_endian_hooks_le/_be(…)`, which
installs all four. In 5/10 MHz mode `dm->security` and `dm->is_net_closed` stay
NULL; phydm dereferences `is_net_closed` unchecked (`phydm_antdiv.c`).

## Fix

Brace the narrowband branch and restore the original statement order: in the
narrowband branch hook BW from `rtw_nb_config` and still install SEC_MODE / BW /
NET_CLOSED (e.g. pass `NULL` for `band` to the Rust helper and have it skip that
one hook), else call the helper with all four.

## Acceptance

- Default L0 unchanged (`CONFIG_NARROWBAND_SUPPORTING` off).
- L0 compile of `hal/hal_dm.o` with `-DCONFIG_NARROWBAND_SUPPORTING`; preprocessed
  output shows SEC_MODE and NET_CLOSED hooks reachable in the narrowband branch.

## Notes

- Pre-existing (keep): narrowband mode already left `band_type` unhooked before the port.
