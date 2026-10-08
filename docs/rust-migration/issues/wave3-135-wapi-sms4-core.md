---
title: "[W3-135] Translate rtw_wapi_sms4.c — SMS4 block cipher core"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-135
epic: E05
blocked_by: [T17]
estimate_loc: 200
---

## Goal

Port the pure SMS4 primitives from [`core/rtw_wapi_sms4.c`](../../../core/rtw_wapi_sms4.c) to
[`rust/rtw_wapi_sms4.rs`](../../../rust/rtw_wapi_sms4.rs):

- `xor_block`, `SMS4Crypt`, `SMS4KeyExt`
- Associated `CK` / S-box tables and `ByteSub` helpers

## Notes

- Depends on **T17** host differential harness for L2 parity before Makefile swap.
- `WAPI_LITTLE_ENDIAN` paths must match C byte order transforms exactly.
- Frame encrypt/decrypt (`rtw_sms4_encrypt` / `decrypt`) stay in C — extract them to
  `core/rtw_wapi_sms4_rest.c` before replacing `core/rtw_wapi_sms4.o` with
  `rust/rtw_wapi_sms4.o` (same `*_rest.c` pattern as `wave3-104`).

## Acceptance

- `rtw_wapi_sms4_rest.c` holds encrypt/decrypt and any symbols not in this slice
- L0 + L1 on swapped object (`rust/rtw_wapi_sms4.o` + `rtw_wapi_sms4_rest.o`)
- L2: `tests/host/wapi_sms4` oracle passes Rust vs C for all vectors
