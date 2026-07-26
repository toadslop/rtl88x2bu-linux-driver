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
- Completes the Rust port of chplan logic started in W2-17…W2-19. Static tables (`RTW_ChannelPlanMap`, `country_chplan_map`, channel defs, …) **remain in C** per epic-04; Rust continues to read them via `extern "C"`.
- **Deferred symbols** (`rtw_process_beacon_hint`, `dump_*` helpers) and **table data** stay in C: extract to `rtw_chplan_rest.c` (same `*_rest.c` pattern as `rtw_swcrypto_rest.c`). Rust ports of those helpers land in tranche 2.

## Acceptance

- L0 build + L1 symbols
- L2 and/or documented contract tests; **L3 VM insmod** recommended (channel init on module probe path)
- No `core/rtw_chplan.o` in CONFIG_RUST build after this slice; static table data, `rtw_process_beacon_hint`, and `dump_*` helpers remain in `rtw_chplan_rest.c` until later issues
