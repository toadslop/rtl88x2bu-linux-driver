---
title: "[W3-185] Translate rtw_pwrctrl.c — FW LPS in IPS"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-185
epic: E05
blocked_by: []
estimate_loc: 180
---

## Goal

Port `CONFIG_FWLPS_IN_IPS` helper from [`core/rtw_pwrctrl.c`](../../../core/rtw_pwrctrl.c) to
[`rust/rtw_pwrctrl_fw_ips.rs`](../../../rust/rtw_pwrctrl_fw_ips.rs):

- `rtw_set_fw_in_ips_mode`

## Notes

- H2C inactive-PS + REG poll loop; btcoex notify on enter — HAL via existing H2C/read8 FFI.
- Guard entire module with `CONFIG_FWLPS_IN_IPS` like C.
- **`CONFIG_FWLPS_IN_IPS` gate:** commented out in [`include/autoconf.h`](../../../include/autoconf.h) for the
  pinned 8822BU profile — L0 on default config; enable the option (or equivalent `-D`) in the PR that
  performs the symbol swap before expecting L1.
- Parallel lane root.

## Acceptance

- L0 build on default pinned config; L0 + L1 when `CONFIG_FWLPS_IN_IPS` is enabled for the translation PR
- L2 optional: stub HAL H2C + poll register for enter/leave enable flag
