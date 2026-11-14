---
title: "[W4-80] Translate hal_intf.c — netdev iface init"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-80
epic: E06
blocked_by: [W4-78]
estimate_loc: 80
---

## Goal

Port `CONFIG_NEW_NETDEV_HDL` `rtw_hal_iface_init` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- MAC addr `set_hwreg`
- `rtw_hal_hw_port_enable` when `RTW_HALMAC`
- `rtw_sec_restore_wep_key`, `rtw_hal_init_opmode`, `rtw_hal_start_thread`

## Notes

- Orchestration only — callee bodies remain in Rust/C per earlier slices.
- Final hal_intf lane slice in tranche 16 unless `rtw_hal_init` is split separately later.

## Acceptance

- L0 with `CONFIG_NEW_NETDEV_HDL=y`
- L1 for `rtw_hal_iface_init`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-78