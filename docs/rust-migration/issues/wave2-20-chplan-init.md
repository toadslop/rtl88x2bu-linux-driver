---
title: "[W2-20] Translate rtw_chplan init_channel_set"
labels: [rust-migration, phase-1, wave-2, size/~200]
type: child
id: W2-20
epic: E04
blocked_by: [W2-18, W2-19]
estimate_loc: 200
---

## Goal

Port adapter-coupled channel-set initialization from [`core/rtw_chplan.c`](../../../core/rtw_chplan.c):

- `init_channel_set_from_rtk_priv`
- `init_channel_set`

## Deferred (tranche 2 — not in this issue)

- `rtw_process_beacon_hint` (~35 LOC, reads `_adapter` / registry state)
- `dump_chplan`, `dump_chset`, and other `dump_*` debug helpers

File separate Wave 3 (or Wave 2 follow-up) issues when adapter/registry characterization is ready.

## Notes

- Touches `_adapter`, `registry_priv`, `hal_chk_band_cap` — expect thin FFI to remaining C HAL helpers.
- Characterize output `RT_CHANNEL_INFO` channel sets per plan id + registry exclusions; L2 where host can mock adapter/registry inputs, else document contract + L3 VM load.
- Final chplan issue: drop `core/rtw_chplan.o` from `rtk_core` when all symbols moved.

## Acceptance

- L0 build + L1 symbols
- L2 and/or documented contract tests; **L3 VM insmod** recommended (channel init on module probe path)
