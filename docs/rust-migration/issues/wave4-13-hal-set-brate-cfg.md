---
title: "[W4-13] Translate hal_com_rest.c — HalSetBrateCfg"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-13
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port basic-rate bitmask construction from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `HalSetBrateCfg`

## Notes

- Pure IEEE 802.11 rate-byte → `RATE_*` bitmask mapping; no adapter MMIO.
- Continues the `hal_com` lane (`rust/hal_com.rs` + `hal_com_rest.c` remainder).
- Independent of W4-10 halmac lane — disjoint Rust module touch set from W4-09/10.

## Acceptance

- L0 + L1 for `HalSetBrateCfg`
- L2: host oracle with OS rate byte arrays (basic CCK/OFDM mixes, empty, non-basic ignored)
