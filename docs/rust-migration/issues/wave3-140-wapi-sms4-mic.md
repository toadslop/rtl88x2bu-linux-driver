---
title: "[W3-140] Translate rtw_wapi_sms4.c — SMS4 MIC calculation"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-140
epic: E05
blocked_by: [W3-139]
estimate_loc: 200
---

## Goal

Port WAPI SMS4 MIC helper from [`core/rtw_wapi_sms4_rest.c`](../../../core/rtw_wapi_sms4_rest.c)
into [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `WapiSMS4CalculateMic`

## Notes

- **Dependency rationale:** MIC path calls into OFB/SMS4 primitives from W3-139.
- `SecCalculateMicSMS4` in C is currently wrapped in `#if 0`; out of scope unless enabling
  that block is required for L2 — focus on `WapiSMS4CalculateMic` first.
- Two-buffer MIC input (`Input1` then `Input2`) with partial final blocks — match C padding.

## Acceptance

- L0 + L1
- L2 host tests for MIC output on fixtures covering single/multi-block `Input1`/`Input2`
