---
title: "[W3-131] Translate rtw_debug.c — dump_drv_cfg Kconfig banner"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-131
epic: E05
blocked_by: [W3-130]
estimate_loc: 200
---

## Goal

Port **part 1** of `dump_drv_cfg` from [`core/rtw_debug.c`](../../../core/rtw_debug.c) to
[`rust/rtw_debug.rs`](../../../rust/rtw_debug.rs) (under `CONFIG_PROC_DEBUG`):

- Kernel/driver version banner through the common `CONFIG_*` blocks (C ~L49–171:
  CFG80211/WEXT, DBG, power saving, phy-from-file, regulatory, adaptivity, wowlan,
  TDLS, 802.11r, netif_sg, wifi_hal, busy-deny-scan, TPT mode).
- **Out of scope here:** USB/SDIO/PCI HCI banners and xmit/recv info — see `W3-138`.

## Notes

- `dump_drv_cfg` is ~200 lines in C; this slice is sized for `size/~200` by splitting
  HCI/xmit tails into `W3-138` (same function, second PR after part 1 lands).
- Part 1 may call a thin C stub for the unported tail until `W3-138` completes, or
  land part 1+138 in one stack — do not parallelize with `W3-138`.
- Reads compile-time `CONFIG_*` values; keep FFI for runtime globals at the edge.
- Register dump paths (`mac_reg_dump`, etc.) stay in C for later slices.

## Acceptance

- L0 build with `CONFIG_PROC_DEBUG=y` in the pinned kernel config
- L1: `dump_drv_cfg` symbol remains exported; proc output matches C baseline for
  the **part 1** banner subset on the pinned `8822BU` USB config
