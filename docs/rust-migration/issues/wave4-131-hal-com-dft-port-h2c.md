---
title: "[W4-131] Translate hal_com_rest.c — default port H2C cmd"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-131
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Under `CONFIG_FW_MULTI_PORT_SUPPORT`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_dft_port.rs`:

- `rtw_hal_set_default_port_id_cmd`
- `rtw_set_default_port_id`

## Notes

- **Parallel lane:** H2C packer only — uses `rtw_hal_get_port`, `rtw_hal_fill_h2c_cmd`, and assoc STA lookup; no register MMIO in this slice.
- Early-out when `(dvobj->dft.port_id, mac_id)` unchanged.
- AP-state branch in `rtw_set_default_port_id` is intentionally empty in C — preserve.

## Acceptance

- L0 + L1 with `CONFIG_FW_MULTI_PORT_SUPPORT` enabled on profile
- L2: stub trace of H2C payload bytes + dvobj `dft` cache updates for STA-associated vs idle paths

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)