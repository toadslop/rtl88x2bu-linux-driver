---
title: "[W3-07] Translate rtw_security.c part 4 — TKIP MIC + phase1/phase2"
labels: [rust-migration, phase-1, wave-3, size/~200]
type: child
id: W3-07
epic: E05
blocked_by: [W3-05]
estimate_loc: 200
---

## Goal

Port TKIP MIC and RC4 key-schedule helpers from [`core/rtw_security.c`](../../../core/rtw_security.c):

- `rtw_secmicsetkey`, `rtw_secmicappendbyte`, `rtw_secmicappend`, `rtw_secgetmic`, `rtw_seccalctkipmic`
- `phase1`, `phase2` (+ `Sbox1` table)

## Notes

- `struct mic_data` crosses FFI — bindgen or explicit `#[repr(C)]` mirror.
- L2: MIC vectors + phase1/phase2 key expansion golden values from C (T5).

## Acceptance

- L0 build + L1 symbols + L2 host vectors (T5)
