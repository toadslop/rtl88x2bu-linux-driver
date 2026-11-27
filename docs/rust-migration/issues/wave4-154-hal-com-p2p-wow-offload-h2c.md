---
title: "[W4-154] Translate hal_com_rest.c — P2P WoWLAN offload H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-154
epic: E06
blocked_by: [W4-153]
estimate_loc: 45
---

## Goal

Under `CONFIG_P2P_WOWLAN`, port `rtw_hal_set_p2p_wowlan_offload_cmd` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_p2p_h2c.rs`.

## Notes

- **In-lane follow-up:** fills `P2P_WoWlan_Offload_t` from `wdinfo` role and WPS config;
  `H2C_P2P_OFFLOAD` via `rtw_hal_fill_h2c_cmd`.
- Shares `rust/hal_com_p2p_h2c.rs` with W4-153 — Makefile swap after P2P rsvd-page H2C.

## Acceptance

- L0 + L1 when `CONFIG_P2P_WOWLAN` enabled
- L2: role byte + WPS config bytes for DEVICE/CLIENT/GO paths

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-153