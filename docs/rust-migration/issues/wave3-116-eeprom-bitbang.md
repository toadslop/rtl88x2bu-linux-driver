---
title: "[W3-116] eeprom bit-bang primitives"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-116
epic: E05
blocked_by: [W3-115]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_eeprom.c`](../../../core/rtw_eeprom.c) to [`rust/rtw_eeprom.rs`](../../../rust/rtw_eeprom.rs):

- `up_clk`
- `down_clk`
- `shift_out_bits`
- `shift_in_bits`
- `standby`
- `wait_eeprom_cmd_done`
- `eeprom_clean`

## Notes

- EEPROM bit-bang clock/shift primitives; first slice of `rtw_eeprom.c`.
- Higher-level read/write APIs ship in W3-117.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for eeprom bit-bang primitives
