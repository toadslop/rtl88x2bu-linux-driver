---
title: "[W3-117] eeprom read/write API"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-117
epic: E05
blocked_by: [W3-116]
estimate_loc: 200
---

## Goal

Port helpers from [`core/rtw_eeprom.c`](../../../core/rtw_eeprom.c) to [`rust/rtw_eeprom.rs`](../../../rust/rtw_eeprom.rs):

- `eeprom_write16`
- `eeprom_read16`
- `eeprom_read_sz`
- `eeprom_read`
- `read_eeprom_content`

## Notes

- EEPROM read/write API layer; builds on W3-116 bit-bang primitives.
- Adapter-coupled paths may need thin C shims or populated fixtures for L2.
- L2: host harness under `tests/host/` with JSON differential vectors (pattern from prior W3 issues).

## Acceptance

- L0 build + L2 host unit tests for eeprom read/write API helpers
