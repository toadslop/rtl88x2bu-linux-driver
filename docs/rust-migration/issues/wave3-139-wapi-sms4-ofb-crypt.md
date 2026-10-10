---
title: "[W3-139] Translate rtw_wapi_sms4.c — SMS4 OFB payload crypt"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-139
epic: E05
blocked_by: [W3-135]
estimate_loc: 200
---

## Goal

Port SMS4 OFB-mode payload helpers from [`core/rtw_wapi_sms4_rest.c`](../../../core/rtw_wapi_sms4_rest.c)
into [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `WapiSMS4Cryption`
- `WapiSMS4Encryption`
- `WapiSMS4Decryption`

## Notes

- **Dependency rationale:** uses `SMS4Crypt` / `SMS4KeyExt` from W3-135; OFB paths call `xor_block`
  from W3-135 (`core/rtw_wapi_sms4.c`) — do not re-port or re-export `xor_block` from this slice.
- Decryption uses the same OFB path as encryption (`ENCRYPT` flag in C); preserve behavior.
- Extract or shrink `core/rtw_wapi_sms4_rest.c` for L1 symbol checks when swapping.
- Extend [`tests/host/wapi_sms4/`](../../../tests/host/wapi_sms4/) (T17) with OFB vectors for
  multi-block and partial final block lengths.

## Acceptance

- L0 + L1
- L2 differential tests for encrypt/decrypt on fixed key/IV/payload fixtures
