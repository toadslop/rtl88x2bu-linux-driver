---
title: "[W4-83] Translate hal_intf.c — xmit/recv priv + SDIO thread dispatch"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-83
epic: E06
blocked_by: [W4-82]
estimate_loc: 100
---

## Goal

Port xmit/recv priv and SDIO thread wrappers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_init_xmit_priv` / `rtw_hal_free_xmit_priv`
- `rtw_hal_init_recv_priv` / `rtw_hal_free_recv_priv`
- `rtw_hal_start_thread` / `rtw_hal_stop_thread` (`CONFIG_SDIO_HCI` / `CONFIG_GSPI_HCI`, honor `CONFIG_SDIO_TX_TASKLET`)

## Notes

- USB-only default profile compiles stubs or empty bodies matching C `#if` gates.
- Chains hal_intf lane after [W4-82](wave4-82-hal-intf-bb-rf-reg.md).

## Acceptance

- L0 on SDIO-disabled USB profile (default cloud pin)
- L1 symbol parity for symbols present in the pinned `.config`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-82