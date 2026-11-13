---
title: "[W4-64] Translate hal_intf.c — chip_rftype_to_hal_rftype"
labels: [rust-migration, phase-1, wave-4, size/~200]
type: child
id: W4-64
epic: E06
blocked_by: [W4-63]
estimate_loc: 120
---

## Goal

Port `rtw_chip_rftype_to_hal_rftype` from [`hal/hal_intf_rest.c`](../../../hal/hal_intf_rest.c) into
[`rust/hal_intf.rs`](../../../rust/hal_intf.rs).

## Notes

- Read `HAL_DATA_TYPE.version_id` through existing `rtw_rust_hal_intf_*` field shims; keep `IS_*T*R` macro semantics identical to C.
- `limit` argument clamps TX/RX path counts before `trx_num_to_rf_type` — mirror `rtw_min` behavior with `wrapping_*` where C wrapped.
- Host L2: freeze version_id fixtures → compare enum results against a small C oracle leaf.

## Acceptance

- L0 + L1 symbol parity for `rtw_chip_rftype_to_hal_rftype`
- L2: host harness vectors for each `IS_*T*R` branch + limit clamp edge cases
