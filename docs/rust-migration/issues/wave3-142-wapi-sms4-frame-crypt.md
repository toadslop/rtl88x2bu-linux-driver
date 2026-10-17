---
title: "[W3-142] Translate rtw_wapi_sms4_rest.c — frame encrypt/decrypt paths"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-142
epic: E05
blocked_by: [W3-136, W3-139, W3-141, T17]
estimate_loc: 200
---

## Goal

Port WAPI SMS4 MPDU encrypt/decrypt entry points from [`core/rtw_wapi_sms4_rest.c`](../../../core/rtw_wapi_sms4_rest.c) (per [W3-135](wave3-135-wapi-sms4-core.md) split — block cipher core stays in `rtw_wapi_sms4.c`) into [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `SecSWSMS4Encryption` / `SecSWSMS4Decryption` and `rtw_sms4_encrypt` / `rtw_sms4_decrypt` adapter hooks
- Software frame paths call Rust `WapiSMS4Encryption` / `WapiSMS4Decryption` ([W3-139](wave3-139-wapi-sms4-ofb-crypt.md)), PN helpers ([W3-141](wave3-141-wapi-pn-qos-cache.md)), and MIC ([W3-140](wave3-140-wapi-sms4-mic.md)) via existing FFI — do not re-port those primitives here
- Makefile / `rtw_wapi_sms4_rest.c` shrink for swapped symbols only

## Notes

- Parallel **wapi_sms4** lane — disjoint from hal_usb / hal_dm.
- **Dependency rationale:** frame encrypt/decrypt orchestrates OFB payload crypt (W3-139), QoS PN cache (W3-141), and IV/PN increment (W3-136); [T17](test-17-host-wapi-sms4-harness.md) supplies algorithm oracles where applicable.
- `_adapter` / `xmitframe` / `recv_frame` at the FFI edge — keep struct access in C shims or thin wrappers as in prior WAPI slices.

## Acceptance

- L0 + L1 on swapped symbols
- L2: host WAPI SMS4 harness vectors for encrypt/decrypt round-trip
