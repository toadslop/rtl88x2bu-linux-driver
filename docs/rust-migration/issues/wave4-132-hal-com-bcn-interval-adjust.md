---
title: "[W4-132] Translate hal_com_rest.c — SW TXBCN interval adjust"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-132
epic: E06
blocked_by: []
estimate_loc: 40
---

## Goal

Under `CONFIG_SWTIMER_BASED_TXBCN`, port `rtw_hal_bcn_interval_adjust` from
[`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c) into `rust/hal_com_bcn_timer.rs`.

## Notes

- **Parallel lane:** pure dvobj field compare — returns `inter_bcn_space` when it differs from requested interval, else input.
- Caller `hw_var_set_bcn_interval` stays C until a later slice; export symbol for L1 swap of this function only.

## Acceptance

- L0 + L1 with `CONFIG_SWTIMER_BASED_TXBCN=y`
- L2: host vectors for match vs mismatch `inter_bcn_space`

## Tracking

- **Epic:** #69 (`E06`)
- **Blocked by:** (none)