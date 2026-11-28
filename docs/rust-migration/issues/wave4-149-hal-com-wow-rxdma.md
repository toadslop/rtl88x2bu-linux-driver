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
- All three helpers are **`static`** under
  `#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)`; callers stay in
  `hal_com_rest.c`. Stock profile has `CONFIG_WOWLAN=n` and `CONFIG_AP_WOWLAN=n`,
  so there are **no** globals to match L1 — do **not** add stub exports (contrast
  W4-145, where the C function is always linked).
- Port as **TU-local** Rust (or `#[no_mangle]` only inside the same cfg guard with no
  extra linker-visible symbols on stock).
- `rtw_hal_pause_rx_dma` uses `s8 trycnt = 100; do { … } while (trycnt--);` — the
  loop body runs for trycnt 100…0 (**101** MMIO polls) before the failure path.

## Acceptance

- L0 on stock profile; L0 when `CONFIG_WOWLAN` or `CONFIG_AP_WOWLAN` is enabled
  (no new global symbols vs stock `hal_com_rest.o` on default L1)
- L2 (WOW profile): stub MMIO trace for pause success vs timeout paths on USB HCI;
  timeout oracle must expect **101** idle polls

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)