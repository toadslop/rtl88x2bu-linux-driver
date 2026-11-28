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

Under `CONFIG_P2P_WOWLAN`, port `rtw_hal_set_FwP2PRsvdPage_cmd` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_p2p_h2c.rs`.

## Notes

- **Parallel lane:** packs five P2P loc offsets via `SET_H2CCMD_RSVDPAGE_LOC_P2P_*`
  then `H2C_P2P_OFFLOAD_RSVD_PAGE`. Default Makefile has `CONFIG_P2P=y` and
  `CONFIG_P2P_WOWLAN=n` — stock `hal_com_rest.o` has **no** symbol; match C guards
  (same pattern as W4-154 acceptance).
- The `RTW_INFO` line prints `LocP2P*` fields; H2C bytes come from **different**
  `RSVDPAGE_LOC` members (preserve exactly):
  - `SET_H2CCMD_RSVDPAGE_LOC_P2P_BCN` ← `LocProbeRsp`
  - `SET_H2CCMD_RSVDPAGE_LOC_P2P_PROBE_RSP` ← `LocPsPoll`
  - `SET_H2CCMD_RSVDPAGE_LOC_P2P_NEGO_RSP` ← `LocNullData`
  - `SET_H2CCMD_RSVDPAGE_LOC_P2P_INVITE_RSP` ← `LocQosNull`
  - `SET_H2CCMD_RSVDPAGE_LOC_P2P_PD_RSP` ← `LocBTQosNull`
- L2 golden vectors must seed those five source fields, not `LocP2P*`.
- Returns `rtw_hal_fill_h2c_cmd` status.

## Acceptance

- L0 on stock profile; L0 + L1 when `CONFIG_P2P_WOWLAN` is enabled
- L2: golden H2C byte layout for seeded non-P2P `RSVDPAGE_LOC` fields above

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)