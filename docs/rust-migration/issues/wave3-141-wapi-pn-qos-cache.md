---
title: "[W3-141] Translate rtw_wapi_sms4.c — QoS PN cache and replay check"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-141
epic: E05
blocked_by: [W3-136]
estimate_loc: 200
---

## Goal

Port WAPI per-QoS PN cache helpers from [`core/rtw_wapi_sms4_rest.c`](../../../core/rtw_wapi_sms4_rest.c)
into [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `WapiGetLastRxUnicastPNForQoSData`
- `WapiSetLastRxUnicastPNForQoSData`
- `WapiCheckPnInSwDecrypt`

## Notes

- **Dependency rationale:** shares PN layout with W3-136 `WapiIncreasePN` / IV helpers.
- `PRT_WAPI_STA_INFO` field access — keep struct layout in bindgen/FFI; pure memcpy/compare logic in Rust.
- `WapiCheckPnInSwDecrypt`: the body that would return `true` is inside `#if 0` in C; live code always
  returns `false`. Rust must preserve always-`false` until that block is intentionally enabled.
- L2: host stubs for `PRT_WAPI_STA_INFO` with fixed queue PN slots; get/set round-trip per UP.

## Acceptance

- L0 + L1
- L2 tests for get/set round-trip per UP; `WapiCheckPnInSwDecrypt` always-`false` (match live C)
