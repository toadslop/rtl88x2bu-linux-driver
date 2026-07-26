---
title: "[W3-05] Translate rtw_chplan.c part 4 — init_channel_set"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-05
epic: E05
blocked_by: [W3-03, W3-04]
estimate_loc: 200
---

## Goal

Port adapter-coupled channel-set initialization from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c):

- `init_channel_set_from_rtk_priv`
- `init_channel_set`

## Notes

- Touches `_adapter`, `registry_priv`, `hal_chk_band_cap` — expect thin FFI to remaining C HAL helpers.
- Characterize output `RT_CHANNEL_INFO` channel sets per plan id + registry exclusions; L2 where host can mock adapter/registry inputs, else document contract + L3 VM load.
- Final W3 chplan issue: drop `core/rtw_chplan.o` from `rtk_core` when all symbols moved.

## Acceptance

- L0 build + L1 symbols
- L2 and/or documented contract tests; **L3 VM insmod** recommended (channel init on module probe path)
