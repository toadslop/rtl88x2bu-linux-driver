---
title: "[W4-47] Translate hal_com_rest.c — C2H register event read/clear"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-47
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Port `CONFIG_FW_C2H_REG` helpers from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `c2h_evt_clear` — `REG_C2HEVT_CLEAR` / `C2H_EVT_HOST_CLOSE`
- `c2h_evt_read_88xx` — trigger validation, ID/SEQ/PLEN/payload MMIO read loop, `clear_evt` path

## Notes

- Independent hal_com lane (no overlap with hal_usb recv init).
- Reuse existing `rtw_read8` / `rtw_write8` FFI from bindgen; macro helpers `SET_C2H_*_88XX` mirrored in Rust.
- Complements [W4-44](wave4-44-hal-usb-c2h-hisr.md) USB-side C2H dispatch.

## Acceptance

- L0 with `CONFIG_FW_C2H_REG=y` on pinned profile
- L1 symbols for `c2h_evt_clear` and `c2h_evt_read_88xx`
- Error paths: NULL buf, invalid trigger, success clears event
