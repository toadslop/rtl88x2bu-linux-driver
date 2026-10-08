---
title: "[W4-04] Translate hal_com.c — rtw_hal_dump_macaddr"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-04
epic: E06
blocked_by: [W4-01]
estimate_loc: 200
---

## Goal

Port `rtw_hal_dump_macaddr` from [`hal/hal_com.c`](../../../hal/hal_com.c) to
[`rust/hal_com.rs`](../../../rust/hal_com.rs).

## Notes

- Reads MAC addresses from adapter / hal data — FFI at boundary; reuse `MacAddr` domain
  type for formatting when safe.
- Requires `rust/hal_com.rs` from `W4-01`; no ordering vs `W4-02`/`W4-03` after scaffold.

## Acceptance

- L0 + L1 symbol parity
- L2 optional with fixed adapter fixture and golden output
