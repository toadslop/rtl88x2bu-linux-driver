---
title: "[W4-150] Translate hal_com_rest.c — WOW enable/disable control check"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-150
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Under `CONFIG_WOWLAN`, port `rtw_hal_check_wow_ctrl` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_wow_ctrl.rs`.

## Notes

- **Parallel lane:** chip-family branches (Jaguar2/3 vs others) for wake-reason
  polling and disable checks — register reads only.
- `chk_type` `_TRUE` = verify WOW enabled; `_FALSE` = verify disabled.
- Keeps `trycnt` sleep loops and diagnostic `RTW_INFO` strings.

## Acceptance

- L0 + L1 with `CONFIG_WOWLAN=y` test profile
- L2: stub register model for enable path clearing wake reason vs disable path stuck bits

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)