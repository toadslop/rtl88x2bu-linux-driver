---
title: "[W4-70] Translate hal_intf.c — interrupt + IPS check wrappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-70
epic: E06
blocked_by: [W4-69]
estimate_loc: 100
---

## Goal

Port interrupt and IPS helper wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_enable_interrupt`
- `rtw_hal_disable_interrupt`
- `rtw_hal_check_ips_status`

## Notes

- SDIO/PCI/GSPI gates on enable/disable — USB-only builds must remain no-ops like C.
- `check_ips_status` logs via `RTW_INFO` when `hal_func.check_ips_status` is NULL; keep format string identical.
- Thin `hal_func` dispatch only — no register access in this slice.

## Acceptance

- L0 + L1 symbol parity for all three symbols
- L3 smoke on pinned kernel QEMU recipe

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-69
