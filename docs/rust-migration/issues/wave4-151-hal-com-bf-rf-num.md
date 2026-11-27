---
title: "[W4-151] Translate hal_com_rest.c — beamforming RF chain count queries"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-151
epic: E06
blocked_by: []
estimate_loc: 50
---

## Goal

Under `CONFIG_BEAMFORMING`, port from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into `rust/hal_com_bf.rs`:

- `rtw_hal_query_txbfer_rf_num`
- `rtw_hal_query_txbfee_rf_num`

## Notes

- **Parallel lane:** registry override vs chip-type defaults for 8822BU/8814AE/8821C.
- 8814AE RF_3T3R / RF_4T4R branches and Broadcom assoc vendor special case on 8814AU.
- Disparate from in-flight `rtw_beamforming.c` wave-3 lanes (no shared Rust files).

## Acceptance

- L0 + L1 with `CONFIG_BEAMFORMING`
- L2: table-driven expectations for registry override, 8822BU default (1), and 8814AE 3T3R

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)