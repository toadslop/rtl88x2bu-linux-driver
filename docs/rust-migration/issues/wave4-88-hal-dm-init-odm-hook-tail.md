---
title: "[W4-88] Translate hal_dm.c — Init_ODM ComInfo hook tail"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-88
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port the remaining **pointer-hook** segment at the end of `Init_ODM_ComInfo` from
[`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs)
(after the Rust helpers from [W4-85](wave4-85-hal-dm-init-odm-adaptivity-hooks.md)):

- `ODM_CMNINFO_CHNL`, scan-in-process, power-saving, TX/RX throughput, antenna-test hooks
- Optional `CONFIG_RTL8723B`, `CONFIG_USB_HCI`, `CONFIG_DYNAMIC_SOML`, `CONFIG_RTW_PATH_DIV` hooks
- `ODM_CMNINFO_FCS_MODE` and `HALRF_CMNINFO_MANUAL_RF_SUPPORTABILITY`
- Optional `CONFIG_MP_INCLUDED` MP-context hooks
- STA info hook loop (`phydm_cmn_sta_info_hook` for `ODM_ASSOCIATE_ENTRY_NUM` indices)
- Tail calls: `rtw_phydm_ops_func_init`, `phydm_dm_early_init`

## Notes

- **Parallel lane:** `rust/hal_dm.rs` only — no hal_intf file overlap while tranche 17 PRs are in flight.
- Band/sec-mode/BW `odm_cmn_info_hook` wiring stays in [W4-38](wave4-38-hal-dm-odm-endian-hooks.md) helpers; this slice starts at `ODM_CMNINFO_CHNL`.
- PHYDM internals (`phydm_dm_early_init`, `phydm_cmn_sta_info_hook`) remain C FFI; Rust orchestrates call order and hook pointers.

## Acceptance

- L0 + L1 for the new `Init_ODM` tail helper invoked from `hal_dm_rest.c`
- L2 spot oracle: fixture adapter/HalData/dvobj fields → expected hook targets for channel, scan, and USB hub speed (USB profile)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)