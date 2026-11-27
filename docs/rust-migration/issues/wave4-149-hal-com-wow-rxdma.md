---
title: "[W4-149] Translate hal_com_rest.c — WOW RX DMA pause/release helpers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-149
epic: E06
blocked_by: []
estimate_loc: 110
---

## Goal

Under `CONFIG_WOWLAN` or `CONFIG_AP_WOWLAN`, port WOW RX-DMA helpers from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_wow_rxdma.rs`:

- `rtw_hal_force_enable_rxdma`
- `rtw_hal_release_rx_dma`
- `rtw_hal_pause_rx_dma`

## Notes

- **Parallel lane:** MMIO on `REG_WOW_CTRL`, `REG_RXPKT_NUM`, optional USB
  `rtw_intf_stop` / `rtw_intf_start` when `CONFIG_USB_HCI`.
- `rtw_hal_pause_rx_dma` polls `RXDMA_IDLE` up to 100 tries — preserve failure log path.
- Export stubs when neither WOWLAN Kconfig is set (same pattern as W4-145).

## Acceptance

- L0 + L1 on stock and WOW-enabled profiles
- L2: stub MMIO trace for pause success vs timeout paths on USB HCI

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)