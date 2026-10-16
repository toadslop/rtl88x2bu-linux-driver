---
title: "[W4-26] Translate hal_halmac.c — phy parameter struct drv→halmac"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-26
epic: E06
blocked_by: [W4-24]
estimate_loc: 200
---

## Goal

Port `_phy_parameter_drv2halmac` from [`hal/hal_halmac.c`](../../../hal/hal_halmac.c) into
[`rust/hal_halmac.rs`](../../../rust/hal_halmac.rs):

- Map `struct rtw_phy_parameter` command/data to `struct halmac_phy_parameter_info`
- MAC / BB / RF register writes, delay (us/ms), and `HALMAC_PARAMETER_CMD_END`
- Reuse `_phy_parameter_val_drv2halmac` from [W4-24](wave4-24-hal-halmac-fw-led-phy-param.md)

## Notes

- Same-file follow-up in the hal_halmac lane after W4-24.
- **Dependency rationale:** struct mapper calls the val helper introduced in W4-24.
- No `rtw_halmac_send_phy_parameter` or live `halmac_adapter` in this slice.

## Acceptance

- L0 + L1 for `_phy_parameter_drv2halmac` (and `#[no_mangle]` alias if the C tree uses it)
- `hal_halmac.c` delegates to Rust when `CONFIG_RUST` + `RTW_HALMAC_RUST_MAPPERS` are enabled
- L2: host vectors for MAC W8/W16/W32, BB W8, RF W, delay us/ms, END, and invalid `cmd` (-1)
