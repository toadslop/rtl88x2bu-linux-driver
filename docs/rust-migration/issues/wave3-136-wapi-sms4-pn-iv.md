---
title: "[W3-136] Translate rtw_wapi_sms4.c — PN increment and SMS4 IV header"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-136
epic: E05
blocked_by: [W3-135]
estimate_loc: 200
---

## Goal

Port WAPI packet-number and IV header helpers from [`core/rtw_wapi_sms4.c`](../../../core/rtw_wapi_sms4.c)
to [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `WapiIncreasePN`
- `SecSMS4HeaderFillIV`

## Notes

- **Dependency rationale:** uses `SMS4Crypt` / key schedule from W3-135.
- `SecSMS4HeaderFillIV` touches `xmitframe` — keep FFI at edge; consider thin C shim for
  struct mutation if needed to stay within ~200 LOC.
- Extend T17 vectors for PN rollover edge cases where practical.

## Acceptance

- L0 + L1
- L2 differential tests for `WapiIncreasePN` and IV fill on fixed fixtures
