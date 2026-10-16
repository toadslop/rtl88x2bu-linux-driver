---
title: "[W4-32] Translate hal_dm.c — dump_sta_traffic debug formatter"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-32
epic: E06
blocked_by: [W4-31]
estimate_loc: 200
---

## Goal

Port `dump_sta_traffic` from [`hal/hal_dm.c`](../../../hal/hal_dm.c) into [`rust/hal_dm.rs`](../../../rust/hal_dm.rs):

- MAC header, optional BCN line when associated to AP, rate/RSSI line, TP and smooth-TP sections
- Mbps vs Kbps formatting (`>> 10` threshold) matching C
- FFI stubs for `is_client_associated_to_ap`, `rtw_get_bcn_*`, and rate helpers

## Notes

- Same hal_dm lane follow-up to [W4-31](wave4-31-hal-dm-dump-sta-info.md).
- Keeps disabled `#if 0` byte counters out of Rust (not compiled in C today).

## Acceptance

- L0 + L1 for `dump_sta_traffic`
- L2: sel-buffer oracle with fixture stats (Mbps and Kbps branches)
