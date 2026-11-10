---
title: "[FX-19] Robustness: c2h_evt_read_88xx length clamp, WapiSMS4Cryption(len=0)"
labels: [rust-migration, phase-1, size/~100]
type: child
id: FX-19
epic: E14
blocked_by: []
estimate_loc: 30
---

## Priority

**P4** — pre-existing C bugs, unreachable in the default build; the Rust failure
mode is now a panic instead of a silent overrun.

## Problem

1. [`rust/hal_com_c2h_evt.rs`](../../../rust/hal_com_c2h_evt.rs) ~86–94 (958f97a6 W4-47 #1398):
   the 8-bit payload length read from register 0x1af indexes a 16-byte buffer from
   offset 2; any length > 12 overflows (C and Rust). Only with `CONFIG_FW_C2H_REG`.
2. [`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs) ~333: `WapiSMS4Cryption`
   with `InputLength == 0` computes `block_num - 1` → C looped `0xFFFFFFFF` times;
   Rust panics (`attempt to subtract with overflow`). Callers are dead SW frame path
   and `#if 0` MIC code.

## Fix

Clamp to `C2H_REG_LEN - 2` (12) before the copy loop; return early with
`*output_length = 0` when `input_length == 0`.

## Acceptance

- L0; L1 unchanged.
- L2: `make -C tests/host/wapi_sms4 all` with a new `len = 0` vector; host vector for
  `c2h_evt_read_88xx` with length 0xFF writes at most 16 bytes.
