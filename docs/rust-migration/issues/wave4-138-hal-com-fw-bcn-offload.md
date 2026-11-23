---
title: "[W4-138] Translate hal_com_rest.c — FW beacon offload H2C pair"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-138
epic: E06
blocked_by: []
estimate_loc: 170
---

## Goal

Under `CONFIG_FW_HANDLE_TXBCN`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_fw_bcn.rs`:

- `rtw_hal_set_fw_ap_bcn_offload_cmd`
- `rtw_hal_set_bcn_rsvdpage_loc_cmd`

## Notes

- **Parallel lane:** updates `dvobj->fw_bcn_offload` / `vap_tbtt_rpt_map` and `vap_map`-driven rsvd page indices; uses `rtw_hal_get_def_var(HAL_DEF_TX_PAGE_SIZE)` + `rtw_hal_fill_h2c_cmd`.
- Does not port `rtw_ap_multi_bcn_cfg` (register storm — separate slice).
- `CONFIG_LIMITED_AP_NUM` loop bounds must match C.

## Acceptance

- L0/L1 on profile with `CONFIG_FW_HANDLE_TXBCN` enabled
- L2: H2C payload + dvobj cache fields vs C for sample `tbtt_rpt_map` and `vap_map` seeds

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)