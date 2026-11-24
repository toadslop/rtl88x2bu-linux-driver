---
title: "[W4-136] Translate hal_com_rest.c — reserved page location H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-136
epic: E06
blocked_by: []
estimate_loc: 80
---

## Goal

Port `rtw_hal_set_FwRsvdPage_cmd` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com_rsvd_page.rs`](../../../rust/hal_com_rsvd_page.rs) (extends W4-121 module).

## Notes

- **Parallel lane:** H2C packer only — maps `RSVDPAGE_LOC` fields into `H2C_RSVD_PAGE` via `SET_H2CCMD_RSVDPAGE_LOC_*` macros; calls `rtw_hal_fill_h2c_cmd`.
- Debug `RTW_INFO` lines for loc offsets should match C format strings.

## Acceptance

- L0 + L1
- L2: stub captures H2C id/len/payload bytes for a seeded `rsvdpageloc` struct

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)