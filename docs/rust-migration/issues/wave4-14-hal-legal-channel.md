---
title: "[W4-14] Translate hal_com_rest.c — HAL_IsLegalChannel"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-14
epic: E06
blocked_by: []
estimate_loc: 200
---

## Goal

Port channel legality checks from [`hal/hal_com_rest.c`](../../../hal/hal_com_rest.c)
into [`rust/hal_com.rs`](../../../rust/hal_com.rs):

- `HAL_IsLegalChannel`

## Notes

- Pure validation: channel number vs `registrypriv.wireless_mode` (2.4G / 5G support).
- No MMIO; continues the `hal_com` lane after W4-13.
- Independent of `hal_phy` / `hal_halmac` lanes — disjoint Rust module.

## Acceptance

- L0 + L1 for `HAL_IsLegalChannel`
- L2: host oracle with stub adapter `wireless_mode` and representative channels (1, 14, 36, 0, 15)
