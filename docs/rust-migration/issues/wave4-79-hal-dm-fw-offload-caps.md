---
title: "[W4-79] Translate hal_dm.c — FW offload capability hooks"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-79
epic: E06
blocked_by: []
estimate_loc: 120
---

## Goal

Port FW offload capability updaters from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into
[`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- `rtw_hal_update_iqk_fw_offload_cap`
- `rtw_hal_update_param_init_fw_offload_cap`

## Notes

- **Parallel lane:** `rust/hal_dm.rs` only — no overlap with hal_intf or hal_com_c2h work.
- Preserve `registrypriv` / `HalData` bit updates and early-return guards from C.

## Acceptance

- L0 + L1 symbol parity
- Optional L2 host mock if a thin oracle is added under `tests/host/hal/` (not required for merge)

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)