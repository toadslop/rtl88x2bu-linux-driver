---
title: "[W4-87] Translate hal_intf.c — xmit enqueue + mgnt xmit dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-87
epic: E06
blocked_by: [W4-83]
estimate_loc: 180
---

## Goal

Port xmit dispatch helpers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_intf_ps_func` (USB autosuspend path)
- `rtw_hal_mgmt_xmitframe_enqueue` (`CONFIG_RTW_MGMT_QUEUE`)
- `rtw_hal_xmitframe_enqueue`, `rtw_hal_xmit`
- `rtw_hal_mgnt_xmit` (attrib updates, optional 802.11w/mesh coalesce, mgmt queue sleeping-sta branch, `hal_func.mgnt_xmit` tail)

## Notes

- Preserve IRQ / lock semantics in the mgmt queue branch (`_enter_critical_bh` / `_exit_critical_bh`).
- Chains hal_intf lane after [W4-83](wave4-83-hal-intf-xmit-recv-thread.md).

## Acceptance

- L0 with default USB profile + optional mgmt-queue Kconfig matrix spot-checked
- L1 symbol parity for compiled symbols

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-83