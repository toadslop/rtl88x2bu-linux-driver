---
title: "[W4-148] Translate hal_com_rest.c — FW debug message rsvd-page H2C"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-148
epic: E06
blocked_by: []
estimate_loc: 30
---

## Goal

Under `DBG_FW_DEBUG_MSG_PKT`, port `rtw_hal_set_fw_dbg_msg_pkt_rsvd_page_cmd` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_fw_dbg_h2c.rs`.

## Notes

- **Parallel lane:** H2C packer only — `SET_H2CCMD_FW_DBG_MSG_PKT_EN` and
  `SET_H2CCMD_RSVDPAGE_LOC_FW_DBG_MSG_PKT` then `H2C_FW_DBG_MSG_PKT`.
- Default profile leaves `DBG_FW_DEBUG_MSG_PKT` undefined; Rust must compile with
  empty export or Kconfig guard matching C.

## Acceptance

- L0 + L1 (symbol present when macro off; body active when macro on)
- L2 when enabled: stub captures H2C payload bytes for seeded `rsvdpageloc`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)