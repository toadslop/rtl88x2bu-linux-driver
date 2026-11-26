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
into `rust/hal_com_fw_aoac_rsvd.rs`.

The C **function export is not `#ifdef CONFIG_WOWLAN`** — only the body is. Default
8822B profile has `CONFIG_WOWLAN=n`; Rust must still link the symbol (empty/stub body
when WOWLAN is off).

## Notes

- **Parallel lane:** separate from W4-136 rsvd-page locator — AOAC-specific H2C ids.
- Associated path (`WIFI_ASOC_STATE`):
  - `H2C_AOAC_RSVD_PAGE` (`0x83`), len `H2C_AOAC_RSVDPAGE_LOC_LEN` (7): remote-wake, ARP,
    neighbor-adv, NDP; then zero buffer + `H2C_AOAC_RSVDPAGE3` (`0x88`) with AOAC report
    at byte 1; set `pwrpriv->wowlan_aoac_rpt_loc = LocAOACReport`. **`H2C_AOAC_RSVD_PAGE2`
    is not used.**
  - On 8822B, `CONFIG_WOWLAN` enables `CONFIG_GTK_OL` (`hal_ic_cfg.h`): GTK rsp/info/ext-mem
    at bytes 3–5 of the `0x83` command via `CONFIG_GTK_OL` SET macros.
- `#ifdef CONFIG_PNO_SUPPORT` **else** branch (non-associated): only `H2C_AOAC_RSVDPAGE3`
  with NLO at byte 0 when `!wowlan_in_resume`; does not set `wowlan_aoac_rpt_loc`.
- `rtw_hal_fill_h2c_cmd` return is stored in `ret` and ignored; C return type is `void`.
- Does not port full WOW ioctl surface (later tranche).

## Acceptance

- L0 + L1 on stock 8822B profile (`CONFIG_WOWLAN=n` symbol present) and WOW-enabled profile
- L2: assoc-path H2C `0x83` / `0x88` packed loc bytes including GTK OL bytes when enabled

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)