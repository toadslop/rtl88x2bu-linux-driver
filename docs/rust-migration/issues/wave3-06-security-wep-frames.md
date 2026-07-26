---
title: "[W3-06] Translate rtw_security.c part 3 — WEP encrypt/decrypt"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-06
epic: E05
blocked_by: [W3-05]
estimate_loc: 200
---

## Goal

Port WEP frame paths from [`core/rtw_security.c`](../../../core/rtw_security.c):

- `rtw_wep_encrypt`
- `rtw_wep_decrypt`

## Notes

- Adapter-coupled (`_adapter *`, xmit/recv frames); thin FFI for frame buffer layout.
- L2: characterize with synthetic frame buffers + keys; differential vs C oracle (T5).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T5)
