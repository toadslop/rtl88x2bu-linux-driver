---
title: "[W3-142] Translate rtw_wapi_sms4.c — frame encrypt/decrypt paths"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-142
epic: E05
blocked_by: [W3-136, T17]
estimate_loc: 200
---

## Goal

Port WAPI SMS4 MPDU encrypt/decrypt entry points from [`core/rtw_wapi_sms4.c`](../../../core/rtw_wapi_sms4.c) (post–[W3-136](wave3-136-wapi-sms4-pn-iv.md) core) into a dedicated `rust/rtw_wapi_sms4.rs` module:

- Software SMS4 frame wrap/unwrap helpers that call existing `WapiSMS4*` primitives already in Rust or C FFI
- Makefile object swap for the encrypt/decrypt symbols only (keep PN/MIC slices separate)

## Notes

- Parallel **wapi_sms4** lane — disjoint from hal_usb / hal_dm.
- **Dependency rationale:** needs SMS4 core + [T17](test-17-host-wapi-sms4-harness.md) L2 vectors for frame-level oracles.

## Acceptance

- L0 + L1 on swapped symbols
- L2: host WAPI SMS4 harness vectors for encrypt/decrypt round-trip
