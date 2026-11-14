---
title: "[W4-76] Translate hal_intf.c — fw_mem_dl + clear_interrupt"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-76
epic: E06
blocked_by: [W4-75]
estimate_loc: 100
---

## Goal

Port conditional HALMAC / WoWLAN helpers from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs):

- `rtw_hal_fw_mem_dl` (`RTW_HALMAC` — timing log + `dbg_fw_mem_dl_error_cnt`)
- `rtw_hal_clear_interrupt` (`CONFIG_WOWLAN` || `CONFIG_AP_WOWLAN`; SDIO/GSPI only)

## Notes

- Use existing Rust shims for `rtw_get_current_time` / `rtw_get_passing_time_ms` where needed.
- Preserve `#if` gates so default USB-only builds do not pull unused symbols.

## Acceptance

- L0 on pinned profile with `RTW_HALMAC=y`
- L1 symbol parity when cfg flags enable each function

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-75