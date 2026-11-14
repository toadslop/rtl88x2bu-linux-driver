---
title: "[W4-81] Translate hal_intf.c — rtw_hal_init orchestration"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-81
epic: E06
blocked_by: [W4-80]
estimate_loc: 180
---

## Goal

Port `rtw_hal_init` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs) for both `CONFIG_NEW_NETDEV_HDL` and legacy paths:

- `halrf_set_rfsupportability` + `hal_func.hal_init` dispatch
- `phydm_init_result` failure branch
- Success path: `rtw_set_hw_init_completed`, notch filter, LED, `init_hw_mlme_ext`, optional HALMAC / multi-BSSID / SOML / PHYDM RRSR hooks preserved via existing C callouts

## Notes

- Orchestration only — chip `hal_init` and PHYDM side effects stay behind existing FFI.
- Same hal_intf lane as tranche 16; stack on the W4-80 PR branch when implementing.
- Split later if post-success hook block grows past the PR size gate.

## Acceptance

- L0 on pinned profile (`CONFIG_NEW_NETDEV_HDL` on + off builds)
- L1 `rtw_hal_init` symbol parity
- L3 smoke: module load still registers driver (no new Oops on `insmod`)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-80