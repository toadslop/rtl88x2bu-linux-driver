---
title: "[W4-106] Translate hal_dm.c — DBG RF cal test wrappers"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-106
epic: E06
blocked_by: [W4-105]
estimate_loc: 120
---

## Goal

Under `CONFIG_DBG_RF_CAL`, port from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_hal_iqk_test`
- `rtw_hal_lck_test`

Preserve PS-deny, ability backup/restore, and `halrf_cmn_info_set` ability masks.

## Notes

- **Dependency rationale:** calls `rtw_phydm_iqk_trigger_dbg` from [W4-105](wave4-105-hal-dm-iqk-trigger-dbg.md).
- Unblocks [W3-230](wave3-230-debug-proc-iqk-lck.md) proc entry once landed.

## Acceptance

- L0 + L1 when `CONFIG_DBG_RF_CAL` is enabled in kconfig
- L2: mock adapter records PS deny / ability restore order and trigger invocations

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-105