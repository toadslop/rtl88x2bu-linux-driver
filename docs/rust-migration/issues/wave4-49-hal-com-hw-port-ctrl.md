---
title: "[W4-49] Translate hal_com_rest.c — RTW_HALMAC hw port enable/disable"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-49
epic: E06
blocked_by: [W4-48]
estimate_loc: 100
---

## Goal

Port `#ifdef RTW_HALMAC` hw port control from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into
[`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `rtw_hal_hw_port_enable` / `rtw_hal_hw_port_disable` — `HW_VAR_PORT_CFG` via `rtw_hal_set_hwreg`
- `rtw_restore_hw_port_cfg` — iterate `dvobj->padapters` and re-enable ports (non-`CONFIG_MI_WITH_MBSSID_CAM` path)

## Notes

- Kconfig-gated in C; Rust export matches `#[cfg(rtw_halmac)]` or Makefile `cfg` used elsewhere in `hal_com.rs`.
- Serial hal_com lane — stack after [W4-48](wave4-48-hal-com-get-port-defaults.md) lands.

## Acceptance

- L0 when `RTW_HALMAC` enabled on pinned profile
- L1 symbols for the three functions
- `extern "C"` signatures unchanged for MI / init callers
