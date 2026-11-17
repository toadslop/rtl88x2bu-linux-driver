---
title: "[W4-110] Translate hal_dm.c — PHYDM HALMAC cfg_phy_para mapper"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-110
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port `rtw_phydm_cfg_phy_para` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs) — map `PHYDM_HALMAC_CMD_*` cases into
`struct rtw_phy_parameter` and call `rtw_halmac_cfg_phy_para`.

## Notes

- **Parallel lane:** complements [W4-109](wave4-109-hal-dm-halmac-dm-leaf.md); largest slice is the MAC/BB/RF/delay switch table.
- RF path enum → `para.data.rf.path` mapping must match C.

## Acceptance

- L0 + L1 symbol parity with HALMAC enabled
- L2: host vectors for each `config_type` build expected `rtw_phy_parameter` blobs (compare to C oracle)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)