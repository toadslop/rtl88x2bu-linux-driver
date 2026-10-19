---
title: "[W4-44] Translate hal_usb.c — usb_c2h_hisr_hdl"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-44
epic: E06
blocked_by: [W4-43]
estimate_loc: 120
---

## Goal

Port `usb_c2h_hisr_hdl` (`CONFIG_FW_C2H_REG`) from [`hal/hal_hci/hal_usb.c`](../../../hal/hal_hci/hal_usb.c) into
[`rust/hal_usb.rs`](../../../rust/hal_usb.rs):

- `rtw_hal_c2h_reg_hdr_parse` delegation
- Direct vs queued handling via `rtw_hal_c2h_id_handle_directly`
- `rtw_cbuf_push` + `_set_workitem` for `c2h_wk`

## Notes

- Thin orchestration around existing HAL/C2H helpers; preserve malloc-fail and NULL-push error paths.
- Chains hal_usb lane after [W4-43](wave4-43-hal-usb-free-recv-priv.md).

## Acceptance

- L0 with `CONFIG_FW_C2H_REG=y` on pinned profile
- L1 symbols for `usb_c2h_hisr_hdl`
- Behavior matches C for direct-handle vs queue branches (code review + optional unit mock)
