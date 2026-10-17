---
title: "[W4-35] Translate hal_phy_rest.c — RF6052 max reg query"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-35
epic: E06
blocked_by: [W4-09]
estimate_loc: 80
---

## Goal

Port `rtw_rust_hal_phy_query_rf_max_reg` from [`hal/hal_phy_rest.c`](../../../hal/hal_phy_rest.c) into
[`rust/hal_phy.rs`](../../../rust/hal_phy.rs):

- Return `GET_RF6052_REAL_MAX_REG(adapter)` semantics (8188E vs default 92C max) via a thin FFI that reads chip type from a fixture adapter in L2

## Notes

- Small slice bundled with RF-shadow module; independent **hal_phy** lane (disjoint from hal_usb / hal_dm).
- C `hal_phy_rest.c` keeps MMIO shadow paths; only this query helper moves.

## Acceptance

- L0 + L1 for `rtw_rust_hal_phy_query_rf_max_reg`
- L2: host test with synthetic adapter flags → expected max reg constant
