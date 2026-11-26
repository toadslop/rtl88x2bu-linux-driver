---
title: "[W4-145] Translate hal_com_rest.c — WOW AOAC reserved-page H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-145
epic: E06
blocked_by: []
estimate_loc: 90
---

## Goal

Port `rtw_hal_set_FwAoacRsvdPage_cmd` from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_fw_aoac_rsvd.rs` under `CONFIG_WOWLAN`.

## Notes

- **Parallel lane:** separate from W4-136 rsvd-page locator — AOAC-specific H2C ids.
- Associated-state path fills `H2C_AOAC_RSVDPAGE` + `H2C_AOAC_RSVDPAGE3` and sets
  `pwrpriv->wowlan_aoac_rpt_loc`.
- `CONFIG_GTK_OL` and `CONFIG_PNO_SUPPORT` sub-branches remain behind matching `cfg` gates in Rust.
- Does not port full WOW ioctl surface (later tranche).

## Acceptance

- L0 + L1 with `CONFIG_WOWLAN=y` on profile
- L2: H2C element ids and packed loc bytes for a seeded `rsvdpageloc` (assoc path)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)