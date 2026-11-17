---
title: "[W4-113] Translate hal_dm.c — PHYDM init orchestration"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-113
epic: E06
blocked_by: [W4-103]
estimate_loc: 100
---

## Goal

Port from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `init_phydm_info` (static helper)
- `rtw_phydm_config_trx_path` (static — `phydm_api_trx_mode` on current `HalData` tx/rx paths)
- `rtw_phydm_init` — calls the helper above, firmware version `cmn_info` init, `odm_dm_init`, optional smart-antenna hook

## Notes

- **Scope boundary:** [W4-103](wave4-103-hal-dm-runtime-trx-path.md) ports only `rtw_hal_runtime_trx_path_decision`; **`rtw_phydm_config_trx_path` lives in this slice.**
- **Dependency rationale:** `blocked_by` W4-103 is merge-order only (same `hal_dm` hot zone), not a functional sub-port of W4-103.
- Does not port watchdog / turbo-EDCA tails.

## Acceptance

- L0 + L1 symbol parity
- L2: mock HAL data records `phydm_init_result` and firmware version fields passed to `odm_cmn_info_init`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** W4-103 (#2116)