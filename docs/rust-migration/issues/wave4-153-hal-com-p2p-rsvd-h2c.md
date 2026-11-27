---
title: "[W4-153] Translate hal_com_rest.c — P2P offload rsvd-page H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-153
epic: E06
blocked_by: []
estimate_loc: 35
---

## Goal

Under `CONFIG_P2P` / P2P WOW build paths, port `rtw_hal_set_FwP2PRsvdPage_cmd` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_p2p_h2c.rs`.

## Notes

- **Parallel lane:** packs five P2P loc offsets via `SET_H2CCMD_RSVDPAGE_LOC_P2P_*`
  macros (note C mapping of `LocProbeRsp` → P2P beacon field — preserve exactly).
- Returns `rtw_hal_fill_h2c_cmd` status.

## Acceptance

- L0 + L1
- L2: golden H2C byte layout for seeded `RSVDPAGE_LOC` offsets

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)