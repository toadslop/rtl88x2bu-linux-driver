---
title: "[W3-07] Translate rtw_security.c part 2 — WEP ARC4/CRC32 primitives"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-07
epic: E05
blocked_by: [W3-06]
estimate_loc: 200
---

## Goal

Port WEP crypto primitives from [`core/rtw_security.c`](../../../core/rtw_security.c):

- `arcfour_init`, `arcfour_byte`, `arcfour_encrypt` (+ `struct arc4context`)
- `crc32_init`, `getcrc32` (+ `crc32_table`)

## Notes

- Pure logic; good L2 candidate. Freeze vectors from C oracle before port.
- Keep static state (`bcrc32initialized`) behavior identical to C.

## Acceptance

- L0 build + L1 symbols + L2 host vectors (extend T2 or new `tests/host/security` crate)
